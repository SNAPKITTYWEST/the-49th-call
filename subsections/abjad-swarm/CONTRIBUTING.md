# Contributing to the SnapKitty Sovereign Stack

![contribution-only](https://img.shields.io/badge/mode-contribution--only-c0392b?style=flat-square)
![sovereign](https://img.shields.io/badge/sovereignty-sealed-8e44ad?style=flat-square)
![node-key](https://img.shields.io/badge/node--key-required-2e86c1?style=flat-square)

---

## This Is Not Open Source

This repository uses the **Sovereign Source License (SSL v3.0)**.

You may:
- **Read** the code
- **Learn** from the architecture
- **Fork** for personal study
- **Contribute** back improvements (PR required, reviewed by sovereign authority)

You may NOT:
- Run in production without a Sovereign Node Key
- Incorporate into proprietary software
- Strip attribution or license headers
- Redistribute modified versions without seal

---

## How to Contribute

1. **Hold a Sovereign Node Key** (see `SOVEREIGN_NODE_KEY.md`)
2. **Fork this repository**
3. **Create a branch** from `main`
4. **Make your changes** — respect the architecture
5. **Submit a Pull Request** — describe what and why
6. **Wait for review** — sovereign authority reviews all PRs

### What We Accept

- Bug fixes with proofs or tests
- Performance improvements with benchmarks
- New backends (ISA targets, MLIR dialects)
- Documentation improvements
- Formal verification contributions (Lean, Agda, Coq)

### What We Reject

- Breaking changes to sealed interfaces
- Dependency additions (we are zero-dep by design)
- AI-generated PRs without human review seal
- Anything that compromises sovereignty or introduces vendor lock-in

---

## Code Standards

- No runtime dependencies. Ever.
- All public functions documented.
- All cryptographic operations use constant-time implementations.
- WORM entries are append-only. Never delete sealed data.
- Every commit message starts with a verb: `add`, `fix`, `seal`, `verify`, `lower`.

---

## Branch Protection

`main` is protected:
- All PRs require review from CODEOWNERS
- Force push is disabled
- Linear history enforced
- Status checks must pass

---

## Attribution

All contributors are logged in the WORM ledger with their node key.
Your contribution is cryptographically sealed and timestamped.
This is better than a CLA — it is a covenant.

---

*Bel Esprit D'Accord Irrevocable Trust*
*SnapKitty West · EIN 42-6976431*
*Evidence or Silence — 2026*
