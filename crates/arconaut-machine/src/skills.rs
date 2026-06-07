use arconaut_core::{ContentPart, Tool, ToolError, ToolResult};
use async_trait::async_trait;
use serde::{Deserialize, Serialize};
use serde_json::Value;
use std::path::{Path, PathBuf};
use std::sync::Arc;

/// Where a skill was discovered.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum SkillSource {
    User,
    Project,
    Path,
}

/// A discovered skill. Content is loaded on demand (Pi pattern).
#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
pub struct Skill {
    pub name: String,
    pub description: String,
    pub file_path: PathBuf,
    pub source: SkillSource,
}

/// Discovers and loads skills from user and project directories.
pub struct SkillLoader {
    user_dir: PathBuf,
    project_dir: PathBuf,
}

impl SkillLoader {
    pub fn new(user_dir: impl Into<PathBuf>, project_dir: impl Into<PathBuf>) -> Self {
        Self {
            user_dir: user_dir.into(),
            project_dir: project_dir.into(),
        }
    }

    /// Discover all skills from both user and project directories.
    /// Deduplicates by file path (a skill found in both directories
    /// prefers the project source).
    pub async fn discover(&self) -> Vec<Skill> {
        let mut skills = Vec::new();
        skills.extend(Self::scan_dir(&self.user_dir, SkillSource::User).await);
        skills.extend(Self::scan_dir(&self.project_dir, SkillSource::Project).await);
        // Deduplicate by file_path, keeping the later (project) source.
        let mut seen = std::collections::HashSet::new();
        skills
            .into_iter()
            .rev()
            .filter(|s| seen.insert(s.file_path.clone()))
            .collect::<Vec<_>>()
            .into_iter()
            .rev()
            .collect()
    }

    /// Load the full content of a skill from disk.
    pub async fn load_content(&self, skill: &Skill) -> std::io::Result<String> {
        tokio::fs::read_to_string(&skill.file_path).await
    }

    async fn scan_dir(dir: &Path, source: SkillSource) -> Vec<Skill> {
        let mut skills = Vec::new();
        let mut entries = match tokio::fs::read_dir(dir).await {
            Ok(e) => e,
            Err(_) => return skills,
        };

        while let Ok(Some(entry)) = entries.next_entry().await {
            let path = entry.path();
            if path.is_file() && path.extension().map(|e| e == "md").unwrap_or(false) {
                if let Some(skill) = Self::skill_from_file(&path, source).await {
                    skills.push(skill);
                }
            } else if path.is_dir() {
                let skill_md = path.join("SKILL.md");
                if skill_md.exists() {
                    if let Some(skill) = Self::skill_from_file(&skill_md, source).await {
                        skills.push(skill);
                    }
                } else {
                    skills.extend(Self::scan_dir_recursive(&path, source).await);
                }
            }
        }
        skills
    }

    async fn scan_dir_recursive(dir: &Path, source: SkillSource) -> Vec<Skill> {
        let mut skills = Vec::new();
        let mut entries = match tokio::fs::read_dir(dir).await {
            Ok(e) => e,
            Err(_) => return skills,
        };

        while let Ok(Some(entry)) = entries.next_entry().await {
            let path = entry.path();
            if path.is_file() && path.extension().map(|e| e == "md").unwrap_or(false) {
                if let Some(skill) = Self::skill_from_file(&path, source).await {
                    skills.push(skill);
                }
            } else if path.is_dir() {
                let skill_md = path.join("SKILL.md");
                if skill_md.exists() {
                    if let Some(skill) = Self::skill_from_file(&skill_md, source).await {
                        skills.push(skill);
                    }
                } else {
                    skills.extend(Box::pin(Self::scan_dir_recursive(&path, source)).await);
                }
            }
        }
        skills
    }

    async fn skill_from_file(path: &Path, source: SkillSource) -> Option<Skill> {
        let content = tokio::fs::read_to_string(path).await.ok()?;
        let (name, description) = Self::parse_frontmatter(&content)?;
        Some(Skill {
            name,
            description,
            file_path: path.to_path_buf(),
            source,
        })
    }

    fn parse_frontmatter(content: &str) -> Option<(String, String)> {
        let trimmed = content.trim_start();
        if !trimmed.starts_with("---") {
            // No frontmatter — use first heading as name, first non-empty body line as description.
            let mut lines = trimmed.lines().map(|l| l.trim());
            let name = lines.next()?.trim_start_matches("# ").to_string();
            let desc = lines.find(|l| !l.is_empty()).unwrap_or("").to_string();
            return Some((name, desc));
        }

        let end = trimmed.find("\n---")? + 1;
        let frontmatter = &trimmed[3..end];
        let name = Self::extract_yaml_value(frontmatter, "name")?;
        let description = Self::extract_yaml_value(frontmatter, "description").unwrap_or_default();
        Some((name, description))
    }

    fn extract_yaml_value(yaml: &str, key: &str) -> Option<String> {
        let prefix = format!("{}:", key);
        yaml.lines()
            .find(|line| line.trim_start().starts_with(&prefix))
            .map(|line| {
                line.split_once(':')
                    .map(|(_, v)| v.trim().to_string())
                    .unwrap_or_default()
            })
            .filter(|v| !v.is_empty())
    }
}

/// Tool that loads and returns skill content by name.
pub struct SkillTool {
    loader: Arc<SkillLoader>,
    params: Value,
}

impl SkillTool {
    pub fn new(loader: Arc<SkillLoader>) -> Self {
        Self {
            loader,
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "name": {
                        "type": "string",
                        "description": "Name of the skill to load"
                    }
                },
                "required": ["name"]
            }),
        }
    }
}

#[async_trait]
impl Tool for SkillTool {
    fn name(&self) -> &str {
        "skill"
    }

    fn description(&self) -> &str {
        "Load a skill file by name. Returns the full skill content. Skills contain specialized instructions for tasks like refactoring, testing, or documentation."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let name = args["name"].as_str().ok_or_else(|| ToolError {
            message: "missing 'name' argument".to_string(),
            brief: "bad args".to_string(),
        })?;

        let skills = self.loader.discover().await;
        let skill = skills.into_iter().find(|s| s.name == name).ok_or_else(|| ToolError {
            message: format!("skill '{}' not found", name),
            brief: "not found".to_string(),
        })?;

        let content = self.loader.load_content(&skill).await.map_err(|e| ToolError {
            message: format!("failed to read skill '{}': {}", name, e),
            brief: "read error".to_string(),
        })?;

        Ok(ToolResult::success(vec![ContentPart::text(content)]))
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::TempDir;

    #[tokio::test]
    async fn discovery_skill_md() {
        let dir = TempDir::new().unwrap();
        let skill_dir = dir.path().join("rust-refactor");
        tokio::fs::create_dir(&skill_dir).await.unwrap();
        tokio::fs::write(
            skill_dir.join("SKILL.md"),
            "---\nname: rust-refactor\ndescription: Refactor Rust code\n---\n\n# Rust Refactoring\n",
        )
        .await
        .unwrap();

        let loader = SkillLoader::new(dir.path(), dir.path());
        let skills = loader.discover().await;
        assert_eq!(skills.len(), 1);
        assert_eq!(skills[0].name, "rust-refactor");
        assert_eq!(skills[0].description, "Refactor Rust code");
    }

    #[tokio::test]
    async fn discovery_recursive() {
        let dir = TempDir::new().unwrap();
        let nested = dir.path().join("deep").join("nested");
        tokio::fs::create_dir_all(&nested).await.unwrap();
        tokio::fs::write(
            nested.join("tip.md"),
            "---\nname: deep-tip\ndescription: A deep tip\n---\n\n# Tip\n",
        )
        .await
        .unwrap();

        let loader = SkillLoader::new(dir.path(), dir.path());
        let skills = loader.discover().await;
        assert_eq!(skills.len(), 1);
        assert_eq!(skills[0].name, "deep-tip");
    }

    #[tokio::test]
    async fn lazy_metadata() {
        let dir = TempDir::new().unwrap();
        tokio::fs::write(
            dir.path().join("test.md"),
            "---\nname: test-skill\ndescription: desc\n---\n\n# Body\nhidden content\n",
        )
        .await
        .unwrap();

        let loader = SkillLoader::new(dir.path(), dir.path());
        let skills = loader.discover().await;
        assert_eq!(skills[0].name, "test-skill");
        // Content should NOT be loaded during discovery.
    }

    #[tokio::test]
    async fn load_content() {
        let dir = TempDir::new().unwrap();
        tokio::fs::write(
            dir.path().join("test.md"),
            "---\nname: test\ndescription: d\n---\n\n# Body\ncontent\n",
        )
        .await
        .unwrap();

        let loader = SkillLoader::new(dir.path(), dir.path());
        let skills = loader.discover().await;
        let content = loader.load_content(&skills[0]).await.unwrap();
        assert!(content.contains("content"));
    }

    #[tokio::test]
    async fn no_frontmatter_fallback() {
        let dir = TempDir::new().unwrap();
        tokio::fs::write(dir.path().join("plain.md"), "# Plain Skill\n\nSome description.\n")
            .await
            .unwrap();

        let loader = SkillLoader::new(dir.path(), dir.path());
        let skills = loader.discover().await;
        assert_eq!(skills.len(), 1);
        assert_eq!(skills[0].name, "Plain Skill");
        assert_eq!(skills[0].description, "Some description.");
    }
}
