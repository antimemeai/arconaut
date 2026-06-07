use arconaut_core::{Context, Message};
use criterion::{black_box, criterion_group, criterion_main, Criterion};

fn bench_append_large_ascii(c: &mut Criterion) {
    let text = "a".repeat(10_000);
    c.bench_function("append_large_ascii", |b| {
        b.iter(|| {
            let mut ctx = Context::new(1_000_000);
            ctx.append_message(Message::user(black_box(&text)));
            black_box(ctx.token_count());
        })
    });
}

fn bench_append_many_small(c: &mut Criterion) {
    let messages: Vec<Message> = (0..100)
        .map(|i| Message::user(format!("message number {}", i)))
        .collect();
    c.bench_function("append_many_small", |b| {
        b.iter(|| {
            let mut ctx = Context::new(1_000_000);
            for msg in &messages {
                ctx.append_message(Message::user(black_box(msg.content[0].as_text().unwrap())));
            }
            black_box(ctx.token_count());
        })
    });
}

fn bench_append_cjk(c: &mut Criterion) {
    let text = "中文字符测试".repeat(500); // 2000 CJK chars
    c.bench_function("append_cjk", |b| {
        b.iter(|| {
            let mut ctx = Context::new(1_000_000);
            ctx.append_message(Message::user(black_box(&text)));
            black_box(ctx.token_count());
        })
    });
}

fn bench_mixed_content(c: &mut Criterion) {
    let text = format!("{}{}", "ascii".repeat(1000), "中文字符".repeat(500));
    c.bench_function("append_mixed_content", |b| {
        b.iter(|| {
            let mut ctx = Context::new(1_000_000);
            ctx.append_message(Message::user(black_box(&text)));
            black_box(ctx.token_count());
        })
    });
}

criterion_group!(
    benches,
    bench_append_large_ascii,
    bench_append_many_small,
    bench_append_cjk,
    bench_mixed_content
);
criterion_main!(benches);
