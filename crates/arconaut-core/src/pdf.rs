use std::path::Path;

/// Minimal markdown-to-PDF converter.
pub struct PdfGenerator;

impl PdfGenerator {
    pub fn new() -> Self {
        Self
    }

    /// Convert markdown text to a PDF file at `output_path`.
    pub fn generate(&self, markdown: &str, output_path: impl AsRef<Path>) -> std::io::Result<()> {
        use pdf_writer::{Content, Name, Pdf, Rect, Ref, Str};

        let mut pdf = Pdf::new();
        let catalog_id = Ref::new(1);
        let page_tree_id = Ref::new(2);
        let page_id = Ref::new(3);
        let content_id = Ref::new(4);
        let font_id = Ref::new(5);

        // Parse markdown into plain text lines with style hints.
        let lines = Self::parse_markdown(markdown);

        // Build PDF content stream.
        let mut content = Content::new();
        content.begin_text();
        content.set_font(Name(b"F1"), 11.0);

        let mut y = 750.0;
        for (style, text) in &lines {
            if y < 50.0 {
                break;
            }
            let size = match style {
                LineStyle::H1 => 18.0,
                LineStyle::H2 => 14.0,
                LineStyle::Code => 9.0,
                LineStyle::Normal => 11.0,
            };
            content.set_font(Name(b"F1"), size);
            content.next_line(50.0, y);
            content.show(Str(text.as_bytes()));
            y -= size + 4.0;
        }
        content.end_text();

        let content_data = content.finish();
        pdf.stream(content_id, &content_data);

        // Page.
        pdf.page(page_id)
            .parent(page_tree_id)
            .media_box(Rect::new(0.0, 0.0, 612.0, 792.0))
            .contents(content_id)
            .resources()
            .fonts()
            .pair(Name(b"F1"), font_id);

        // Page tree.
        pdf.pages(page_tree_id).kids([page_id]).count(1);

        // Font (standard Helvetica, no embedding needed).
        pdf.type1_font(font_id).base_font(Name(b"Helvetica"));

        // Catalog.
        pdf.catalog(catalog_id).pages(page_tree_id);

        std::fs::write(output_path, pdf.finish())?;
        Ok(())
    }

    fn parse_markdown(text: &str) -> Vec<(LineStyle, String)> {
        use pulldown_cmark::{Event, HeadingLevel, Tag, TagEnd};
        let mut lines = Vec::new();
        let parser = pulldown_cmark::Parser::new(text);
        let mut current_text = String::new();
        let mut current_style = LineStyle::Normal;

        for event in parser {
            match event {
                Event::Start(tag) => match tag {
                    Tag::Heading { level, .. } => {
                        if !current_text.is_empty() {
                            lines.push((current_style, current_text.trim().to_string()));
                            current_text.clear();
                        }
                        current_style = match level {
                            HeadingLevel::H1 => LineStyle::H1,
                            _ => LineStyle::H2,
                        };
                    }
                    Tag::CodeBlock(_) => {
                        if !current_text.is_empty() {
                            lines.push((current_style, current_text.trim().to_string()));
                            current_text.clear();
                        }
                        current_style = LineStyle::Code;
                    }
                    _ => {}
                },
                Event::End(TagEnd::Heading(_) | TagEnd::CodeBlock) => {
                    if !current_text.is_empty() {
                        lines.push((current_style, current_text.trim().to_string()));
                        current_text.clear();
                    }
                    current_style = LineStyle::Normal;
                }
                Event::Text(t) => {
                    current_text.push_str(&t);
                }
                Event::Code(c) => {
                    current_text.push_str(&c);
                }
                Event::SoftBreak | Event::HardBreak => {
                    current_text.push(' ');
                }
                _ => {}
            }
        }
        if !current_text.is_empty() {
            lines.push((current_style, current_text.trim().to_string()));
        }
        lines
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum LineStyle {
    Normal,
    H1,
    H2,
    Code,
}

impl Default for PdfGenerator {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::TempDir;

    #[test]
    fn parse_markdown_events() {
        let md = "# Heading\n\nParagraph text.\n\n```\ncode block\n```\n";
        let lines = PdfGenerator::parse_markdown(md);
        assert!(lines.iter().any(|(s, t)| *s == LineStyle::H1 && t == "Heading"));
        assert!(lines.iter().any(|(s, t)| *s == LineStyle::Normal && t == "Paragraph text."));
        assert!(lines.iter().any(|(s, t)| *s == LineStyle::Code && t == "code block"));
    }

    #[test]
    fn generate_pdf() {
        let dir = TempDir::new().unwrap();
        let path = dir.path().join("test.pdf");
        let generator = PdfGenerator::new();
        generator.generate("# Report\n\nHello world.", &path).unwrap();
        assert!(path.exists());
        let data = std::fs::read(&path).unwrap();
        assert!(data.starts_with(b"%PDF"));
    }
}
