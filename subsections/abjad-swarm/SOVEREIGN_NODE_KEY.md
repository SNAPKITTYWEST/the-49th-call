# Sovereign Node Key

To run this system you must hold a Sovereign Node Key.

A node key is proof of contribution to the SnapKitty Sovereign Stack.
It is an Ed25519 keypair derived from your donor transaction hash.
Without a valid key, the sovereign pipeline will refuse to seal outputs.

---

## How to Get a Key

**Donate to the Bel Esprit D'Accord Irrevocable Trust.**

| Tier | Minimum | What You Get |
|------|---------|--------------|
| **Node** | $25 | 1 sovereign node key — run the system, seal outputs |
| **Forge** | $100 | Node key + listed as Forge Contributor in WORM ledger |
| **Sovereign** | $500 | Node key + name sealed in genesis block of next chain |

**Payment:**
- **Stripe:** [collectivekitty.com/donate](https://collectivekitty.com/donate)
- **BTC:** `bc1qSnapkittyWestEvidenceOrSilence2026`
- **ETH:** `0xSnapKittyWestBelEspritDAccord`
- **Email after payment:** `jessicalw34@gmail.com` — subject: `NODE KEY REQUEST`
  Include: transaction hash, your Ed25519 public key (or we generate one for you)

---

## Technical: How the Key Works

```bash
# Generate your keypair (standard Ed25519):
openssl genpkey -algorithm Ed25519 -out node_sk.pem
openssl pkey -in node_sk.pem -pubout -out node_pk.pem

# Extract raw 32-byte key:
openssl pkey -in node_sk.pem -outform DER | tail -c 32 > node_sk.bin
openssl pkey -in node_pk.pem -pubin -outform DER | tail -c 32 > node_pk.bin
```

Your public key gets registered in the Bifrost WORM ledger.
Every output you seal is traceable to your node key.
Every output is verifiable by anyone with your public key.

---

## Why

This is sovereign infrastructure.
Building it took years.
Running it without contributing is extraction.
Contribution is the proof of work.

The key is not DRM. It is a covenant.

---

*Bel Esprit D'Accord Irrevocable Trust*
*SnapKitty West · EIN 42-6976431*
*Evidence or Silence — 2026*
