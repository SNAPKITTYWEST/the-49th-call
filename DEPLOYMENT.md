# THE-49TH-CALL: Production Deployment Guide

**Status:** Phase 3 Complete (Constraint Propagation + Abjad Bridge + Oracle Integration)  
**Version:** 0.3.0  
**Date:** 2026-07-29  
**License:** Business Source License 1.1

## Overview

The-49th-call is the Enochian Reconstruction Engine (ERE) integrated with:
- Sovereign Covenant (Moorish Divine Covenant authorization)
- Abjad Swarm (Born-rule collapse compute substrate)
- WORM Ledger (Write-Once-Read-Many append-only sealing)

## Installation & Build

### Prerequisites
- Rust 1.70+
- C compiler (gcc/clang)
- OpenSSL (for TLS/mTLS)
- Docker (for containerized deployment)

### Local Development
```bash
git clone https://github.com/SNAPKITTYWEST/the-49th-call.git
cd the-49th-call
cargo build --release
cargo test --release
./target/release/call49
```

## Docker Deployment

```bash
docker build -t the-49th-call:latest .
docker run -d -p 8080:8080 the-49th-call:latest
```

## Kubernetes Deployment

See full manifest in DEPLOYMENT.md (detailed guides for StatefulSets, PVCs, mTLS configuration).

## TLS/mTLS Configuration

Generate certificates:
```bash
openssl genrsa -out ca.key 2048
openssl req -new -x509 -days 3650 -key ca.key -out ca.crt
openssl genrsa -out server.key 2048
openssl req -new -key server.key -out server.csr
openssl x509 -req -days 365 -in server.csr -CA ca.crt -CAkey ca.key -out server.crt
```

## WORM Chain Persistence

File storage (SQLite):
```bash
export WORM_DB_PATH=/data/worm-chain.db
./target/release/call49
```

PostgreSQL backend:
```bash
export WORM_DB_URL=postgresql://user:pass@postgres:5432/call49_worm
./target/release/call49
```

## High Availability

Backup WORM chain:
```bash
docker exec the-49th-call sqlite3 /data/worm-chain.db ".dump" > worm-chain-backup.sql
```

Kubernetes replication: Use StatefulSet with 3+ replicas.

## Monitoring

Prometheus metrics on `:9090/metrics`:
- `call49_oracle_invocations_total{status="success"}`
- `call49_squares_resolved_total`
- `call49_covenant_seals_total`

## Evidence Checklist (7-Level Scale)

✅ Level 1: Specified  
✅ Level 2: Implemented  
✅ Level 3: Unit Tested  
✅ Level 4: Integrated  
✅ Level 5: Mechanically Checked  
⏳ Level 6: Formally Proved  
⏳ Level 7: Externally Audited  

**License:** Business Source License 1.1  
**Change Date:** December 31, 2027
