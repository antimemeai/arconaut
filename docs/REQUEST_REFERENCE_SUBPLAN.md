# Native admission request reference

Store each CodingEngine operation input once in its invocation. An explicitly marked admission
uses retained schema 2 with only attempt/invocation/decision identities. Schema 1
inline admissions remain readable and writable for consumers with distinct admission inputs.
Ledger admission resolves the reference against an earlier valid invocation;
execution and inspection receive its immutable shared bytes. Historical readers
resolve through the retained archive with their own storage lifetime. No replay
policy changes. Older binaries refuse schema 2 rather than interpreting it.

Direct checks: exact admission wire length; completed and interrupted native
provider reopen, preserved input and zero effect redispatch; legacy codec and
retained-state cases. Allowance 45 minutes, one recheck after implementation.
