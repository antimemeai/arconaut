use chrono::{DateTime, Utc};
use std::path::{Path, PathBuf};

/// A discovered document or report.
#[derive(Debug, Clone, PartialEq)]
pub struct Document {
    pub name: String,
    pub path: PathBuf,
    pub kind: DocumentKind,
    pub created_at: Option<DateTime<Utc>>,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DocumentKind {
    Markdown,
    Pdf,
    Text,
    Other,
}

impl DocumentKind {
    fn from_path(path: &Path) -> Self {
        match path.extension().and_then(|e| e.to_str()) {
            Some("md") | Some("markdown") => DocumentKind::Markdown,
            Some("pdf") => DocumentKind::Pdf,
            Some("txt") | Some("rs") | Some("py") | Some("js") | Some("ts")
            | Some("json") | Some("toml") | Some("yaml") | Some("yml") => DocumentKind::Text,
            _ => DocumentKind::Other,
        }
    }
}

/// Scans directories for documents and reports.
pub struct DocumentIndex {
    dirs: Vec<PathBuf>,
}

impl DocumentIndex {
    pub fn new(dirs: Vec<PathBuf>) -> Self {
        Self { dirs }
    }

    /// Scan all registered directories for documents.
    pub async fn scan(&self) -> Vec<Document> {
        let mut docs = Vec::new();
        for dir in &self.dirs {
            docs.extend(Self::scan_dir(dir).await);
        }
        docs
    }

    /// Filter documents by kind.
    pub fn filter_kind(docs: &[Document], kind: DocumentKind) -> Vec<Document> {
        docs.iter()
            .filter(|d| d.kind == kind)
            .cloned()
            .collect()
    }

    /// Sort documents by creation time (newest first).
    pub fn sort_by_date(docs: &mut [Document]) {
        docs.sort_by(|a, b| b.created_at.cmp(&a.created_at));
    }

    async fn scan_dir(dir: &Path) -> Vec<Document> {
        let mut docs = Vec::new();
        let mut entries = match tokio::fs::read_dir(dir).await {
            Ok(e) => e,
            Err(_) => return docs,
        };

        while let Ok(Some(entry)) = entries.next_entry().await {
            let path = entry.path();
            if path.is_file() {
                let name = path
                    .file_stem()
                    .and_then(|s| s.to_str())
                    .unwrap_or("unknown")
                    .to_string();
                let kind = DocumentKind::from_path(&path);
                let created_at = entry
                    .metadata()
                    .await
                    .ok()
                    .and_then(|m| m.created().ok())
                    .and_then(|t| DateTime::from_timestamp(t.duration_since(std::time::UNIX_EPOCH).ok()?.as_secs() as i64, 0));
                docs.push(Document {
                    name,
                    path,
                    kind,
                    created_at,
                });
            }
        }
        docs
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::TempDir;

    #[tokio::test]
    async fn scan_finds_files() {
        let dir = TempDir::new().unwrap();
        tokio::fs::write(dir.path().join("readme.md"), "# Hello")
            .await
            .unwrap();
        tokio::fs::write(dir.path().join("notes.txt"), "notes")
            .await
            .unwrap();

        let index = DocumentIndex::new(vec![dir.path().to_path_buf()]);
        let docs = index.scan().await;
        assert_eq!(docs.len(), 2);
    }

    #[tokio::test]
    async fn filter_kind() {
        let dir = TempDir::new().unwrap();
        tokio::fs::write(dir.path().join("a.md"), "").await.unwrap();
        tokio::fs::write(dir.path().join("b.txt"), "").await.unwrap();

        let index = DocumentIndex::new(vec![dir.path().to_path_buf()]);
        let docs = index.scan().await;
        let md = DocumentIndex::filter_kind(&docs, DocumentKind::Markdown);
        assert_eq!(md.len(), 1);
        assert_eq!(md[0].name, "a");
    }

    #[tokio::test]
    async fn sort_by_date() {
        let dir = TempDir::new().unwrap();
        tokio::fs::write(dir.path().join("old.md"), "").await.unwrap();
        tokio::fs::write(dir.path().join("new.md"), "").await.unwrap();

        let index = DocumentIndex::new(vec![dir.path().to_path_buf()]);
        let mut docs = index.scan().await;
        DocumentIndex::sort_by_date(&mut docs);
        // Order depends on filesystem timestamps; just verify it doesn't panic.
    }
}
