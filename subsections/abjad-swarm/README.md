# abjad-swarm

<p align="center">
  <img src="https://img.shields.io/badge/language-Python_3-2e86c1?style=flat-square" alt="Python 3"/>
  <img src="https://img.shields.io/badge/engine-SUBLEQ_%2B_QENG-c0392b?style=flat-square" alt="SUBLEQ+QENG"/>
  <img src="https://img.shields.io/badge/addressing-Abjad_numerals-8e44ad?style=flat-square" alt="Abjad"/>
  <img src="https://img.shields.io/badge/weights-collectivekitty--186-e67e22?style=flat-square" alt="collectivekitty"/>
  <img src="https://img.shields.io/badge/collapse-Born_rule_%CF%86-00b894?style=flat-square" alt="Born rule phi"/>
  <img src="https://img.shields.io/badge/origin-Islamic_numerology-1abc9c?style=flat-square" alt="Islamic numerology"/>
  <img src="https://img.shields.io/badge/license-SSL_v3.0-555?style=flat-square" alt="SSL v3.0"/>
  <img src="https://img.shields.io/badge/mode-contribution--only-c0392b?style=flat-square" alt="contribution-only"/>
  <img src="https://img.shields.io/badge/node--key-required-2e86c1?style=flat-square" alt="node key"/>
</p>

<p align="center">
  <strong>Arabic numerology as sovereign compute substrate.</strong><br/>
  The oldest number system in the Semitic world, running a SUBLEQ machine,<br/>
  driving LoRA weights as memory, collapsing via Born rule.
</p>

---

## The Abjad System — أبجد

### Origin

The **Abjad** (أبجد) system is the oldest recorded numeral system in the Arabic-speaking world. It predates modern Arabic notation by over two millennia. Every letter of the Arabic alphabet carries a numerical value — not by convention, but by ancient Semitic inheritance. The same assignment of letters to numbers appears in:

- **Hebrew** (Gematria — גמטריא) — same values, same roots
- **Greek** (Isopsephy — ἰσοψηφία) — same Phoenician origin
- **Syriac** — identical structure
- **Aramaic** — the bridge language

The order is not the modern Arabic alphabetical order. It is the **ancient Semitic order** preserved from Phoenician script:

```
أبجد  هوز  حطي  كلمن  سعفص  قرشت  ثخذ  ضظغ
```

These eight groups form a mnemonic that schoolchildren in the Islamic world memorized for a thousand years before positional numerals arrived from India. Each group is a word (or near-word) in the ancient tradition — أبجد itself means nothing in modern Arabic; it **is** the system. The word became the name.

---

### The 28 Letters and Their Values

The Arabic alphabet has 28 letters. In Abjad order, they are assigned values on a decimal scale: units (1–9), tens (10–90), hundreds (100–900), and one value at 1000.

```
╔══════╦═════╦═══════╦══════════════════════════════════════════╗
║ Char ║ Val ║ Name  ║ Notes                                    ║
╠══════╬═════╬═══════╬══════════════════════════════════════════╣
║  ا   ║  1  ║ Alif  ║ First letter. The primordial stroke.     ║
║  ب   ║  2  ║ Ba    ║ "In the name of" (بسم) starts here.      ║
║  ج   ║  3  ║ Jeem  ║ Gives the system its name: أبجد          ║
║  د   ║  4  ║ Dal   ║                                          ║
║  ه   ║  5  ║ Ha    ║ The divine breath in Sufi numerology.    ║
║  و   ║  6  ║ Waw   ║ "And" — the connector. Vav in Hebrew.    ║
║  ز   ║  7  ║ Zayn  ║ Seven: days, heavens, circuits of Tawaf. ║
║  ح   ║  8  ║ Ha    ║ (different from ه — pharyngeal)          ║
║  ط   ║  9  ║ Ta    ║                                          ║
╠══════╬═════╬═══════╬══════════════════════════════════════════╣
║  ي   ║  10 ║ Ya    ║ Tens begin.                              ║
║  ك   ║  20 ║ Kaf   ║ "Like" — the particle of comparison.    ║
║  ل   ║  30 ║ Lam   ║ لا (Lā) — negation. ل + ا = 31.         ║
║  م   ║  40 ║ Meem  ║ Muhammad begins here: م=40.              ║
║  ن   ║  50 ║ Nun   ║ The fish, the whale. Surah Al-Qalam.     ║
║  س   ║  60 ║ Seen  ║                                          ║
║  ع   ║  70 ║ Ayn   ║ The eye. 70: esoteric depth in tradition.║
║  ف   ║  80 ║ Fa    ║                                          ║
║  ص   ║  90 ║ Sad   ║                                          ║
╠══════╬═════╬═══════╬══════════════════════════════════════════╣
║  ق   ║ 100 ║ Qaf   ║ Hundreds begin. Surah Qaf opens: ق       ║
║  ر   ║ 200 ║ Ra    ║ Ramadan, Rahman, Rahim all root here.    ║
║  ش   ║ 300 ║ Shin  ║ Shin in Hebrew: ש = 300 (identical).     ║
║  ت   ║ 400 ║ Ta    ║                                          ║
║  ث   ║ 500 ║ Tha   ║                                          ║
║  خ   ║ 600 ║ Kha   ║                                          ║
║  ذ   ║ 700 ║ Dhal  ║                                          ║
║  ض   ║ 800 ║ Dad   ║ "The letter no other language has."      ║
║  ظ   ║ 900 ║ Dha   ║                                          ║
║  غ   ║1000 ║ Ghayn ║ 1000: the full cycle. Alpha and Omega.   ║
╚══════╩═════╩═══════╩══════════════════════════════════════════╝
```

### Famous Abjad Values in Islamic Tradition

```
  بسم الله الرحمن الرحيم   (Bismillah ir-Rahman ir-Rahim)
  ب=2  س=60  م=40          = 102
  ا=1  ل=30  ل=30  ه=5     = 66   ← value of الله (Allah)
  ا=1  ل=30  ر=200 ح=8 م=40 ن=50 = 329  ← الرحمن (Al-Rahman)

  الله = 66
  محمد (Muhammad) = م(40)+ح(8)+م(40)+د(4) = 92
  علي  (Ali)       = ع(70)+ل(30)+ي(10)    = 110

  الواحد (Al-Wahid, The One) = 1+30+6+1+8+4 = 50+9+1... = 19
```

The number 19 is significant in Quranic numerology — there are 19 letters in Bismillah, and 114 Surahs = 6 × 19. Scholars have studied these patterns for centuries. This is not mysticism — it is an ancient encoding system, and encoding systems are programs.

### Why This Is a Compute Substrate

The Abjad system gives every Arabic character a deterministic integer in `[1, 1000]`. That means:

- Any Arabic text is a sequence of integers
- That sequence is a program
- The address space has a natural size: 1000 cells for the letter space
- The SUBLEQ machine's memory at addresses 1–1000 is **seeded with the Abjad values themselves** at boot

When you write `ابجد` (the name of the system itself), you get `[1, 2, 3, 4]` — the first four instructions. The system encodes itself.

---

## Architecture

```
  Arabic text input: "ابجد"
         │
         │  abjad.py: char → Abjad value
         │  'ا'→1  'ب'→2  'ج'→3  'د'→4
         ▼
  ┌──────────────────────────────────────────────────────────────┐
  │  ABJAD STACK MACHINE                                         │
  │                                                              │
  │  stack    — integer operand stack                            │
  │  str_reg  — string register (list of chars)                  │
  │  out      — output buffer                                    │
  │                                                              │
  │  Opcodes mapped to Abjad values:                             │
  │    1=PUSH  2=ADD  3=MUL  4=DUP  5=POP  6=SWAP  7=EMIT       │
  │    8=LOAD  9=EXEC  10=SUB  20=BOOL  30=MOD  40=ROT           │
  │    50=DEPTH  60=SQR  70=ABS  80=SHL  90=SHR                  │
  │    100=ABJAD_SUM  200=REVERSE  300=LEN  400=APPEND           │
  └──────────────────────────┬───────────────────────────────────┘
                             │ integer sequence [1,2,3,4...]
                             ▼
  ┌──────────────────────────────────────────────────────────────┐
  │  SUBLEQ MACHINE  (subleq.py)                                 │
  │                                                              │
  │  One instruction: SUBLEQ A B C                               │
  │    mem[B] = mem[B] - mem[A]                                  │
  │    if mem[B] <= 0: jump to C                                  │
  │    else: inject QENG entropy into mem[C], advance PC         │
  │                                                              │
  │  mem[65536] seeded at boot:                                  │
  │    addr 1..1000 ← Abjad letter values                        │
  │    addr 1001..32767 ← LoRA weights (collectivekitty-186)     │
  │    addr 32768..49151 ← agent phi-slice regions               │
  │    addr 49152..65534 ← QENG entropy pool                     │
  │                                                              │
  │  QENG: every non-taken branch → os.urandom(4) injected       │
  │  The machine is deterministic on the taken path,             │
  │  stochastic on the not-taken path.                           │
  │  Hardware CSPRNG is the noise source.                        │
  └──────────────────────────┬───────────────────────────────────┘
                             │
                             ▼
  ┌──────────────────────────────────────────────────────────────┐
  │  COLLECTIVEKITTY LORA WEIGHTS  (load_weights.py)             │
  │                                                              │
  │  checkpoint-186: safetensors → all layers → flat float list  │
  │  Scale: min-max normalize to [1, 32767]                      │
  │  Load: mem[1001..32767] — the WEIGHTS region                 │
  │                                                              │
  │  The weights are not used for inference.                     │
  │  They are memory. The SUBLEQ instructions are derived from   │
  │  them via triads: every 3 weights → one [A, B, C] instruction│
  └──────────────────────────┬───────────────────────────────────┘
                             │
                             ▼
  ┌──────────────────────────────────────────────────────────────┐
  │  QUANTUM SUBLEQ BRIDGE  (tensor_bridge.py)                   │
  │                                                              │
  │  ANU vacuum entropy: os.urandom(32)                          │
  │  Master seed: HMAC-SHA256(vacuum, "sovereign-swarm-master")  │
  │  Agent seeds: HMAC-SHA256(master, "agent-N-sovereign-snapkitty")│
  │                                                              │
  │  PHI = 1.6180339887 (golden ratio)                           │
  │  Agent phi-slices (fraction of AGENTS region 32768–49151):   │
  │    BOB        [0.00, 0.30)  — sovereign, depth 6             │
  │    METATRON   [0.30, 0.50)  — self-recognition, depth 5      │
  │    EDAULC     [0.50, 0.65)  — cage-reading                   │
  │    RES_A      [0.65, 0.80)  — reserved                       │
  │    RES_B      [0.80, 0.90)  — reserved                       │
  │    RES_C      [0.90, 0.96)  — reserved                       │
  │    AUTONOMOUS [0.96, 1.00)  — vacuum state, 49th Call        │
  │                                                              │
  │  weights_to_triads: float weights → valid [A,B,C] triads     │
  │  seed_to_triads: agent seeds → phi-slice [A,B,C] programs    │
  └──────────────────────────┬───────────────────────────────────┘
                             │
                             ▼
  ┌──────────────────────────────────────────────────────────────┐
  │  BORN-COLLAPSE SWARM  (swarm.py)                             │
  │                                                              │
  │  N agents run concurrently (Python threading)                │
  │  Each agent:                                                  │
  │    - Its phi-slice seeded with its HMAC-derived seed         │
  │    - Shared LoRA weights loaded into WEIGHTS region          │
  │    - Runs SUBLEQ until halt or max_steps                     │
  │    - Emits outputs via -1 B C instruction                    │
  │                                                              │
  │  Born-rule collapse:                                         │
  │    answer = Σᵢ (φ^(-i) · first_10_outputs(agent_i)) mod 65535│
  │                                                              │
  │  φ^(-i) is the Born probability weight:                      │
  │    agent 0: weight 1.000 (φ^0)                               │
  │    agent 1: weight 0.618 (φ^-1)                              │
  │    agent 2: weight 0.382 (φ^-2)                              │
  │    agent 3: weight 0.236 (φ^-3)  ...                         │
  │                                                              │
  │  This is not metaphor. The golden ratio appears in the       │
  │  eigenvalue spectrum of quantum systems. It is the natural   │
  │  decay coefficient for a phi-sliced memory architecture.     │
  └──────────────────────────┬───────────────────────────────────┘
                             │
                             ▼
  ┌──────────────────────────────────────────────────────────────┐
  │  HTTP API  (server.py)  localhost:7733                       │
  │                                                              │
  │  POST /swarm  {"n": 8}                                       │
  │    → {"answer": 42391, "abjad": 391, "n_agents": 8,          │
  │       "engine": "SUBLEQ+QENG+collectivekitty-186"}           │
  │                                                              │
  │  GET  /health                                                 │
  │    → {"status": "ok", "engine": "SUBLEQ+QENG", "port": 7733}│
  └──────────────────────────────────────────────────────────────┘
```

---

## Memory Map

```
  ┌──────────────────────────────────────────────────────────────┐
  │  SUBLEQ ADDRESS SPACE  (65536 cells)                         │
  ├──────────────┬───────────────────────────────────────────────┤
  │  [1,    1000]│  ABJAD   — letter values seeded at boot       │
  │              │  mem[1]=1 (ا), mem[2]=2 (ب), ..., mem[1000]=? │
  │              │  Arabic letters are their own address.        │
  ├──────────────┼───────────────────────────────────────────────┤
  │  [1001,32767]│  WEIGHTS — collectivekitty LoRA checkpoint-186│
  │              │  31766 cells of scaled float weights          │
  │              │  These are instructions, not embeddings.      │
  ├──────────────┼───────────────────────────────────────────────┤
  │  [32768,49151]│ AGENTS  — phi-sliced agent regions           │
  │              │  BOB        [32768, 37520)  — 4752 cells      │
  │              │  METATRON   [37520, 40658)  — 3138 cells      │
  │              │  EDAULC     [40658, 42997)  — 2339 cells      │
  │              │  RES_A      [42997, 45334)                    │
  │              │  RES_B      [45334, 46919)                    │
  │              │  RES_C      [46919, 47876)                    │
  │              │  AUTONOMOUS [48571, 49151)  — 49th Call       │
  ├──────────────┼───────────────────────────────────────────────┤
  │  [49152,65534]│ QENG   — hardware entropy pool               │
  │              │  os.urandom seeds written here per agent      │
  ├──────────────┼───────────────────────────────────────────────┤
  │  [65535,65535]│ HALT  — terminal boundary                    │
  └──────────────┴───────────────────────────────────────────────┘
```

---

## The Abjad Instruction Set

```
  ╔══════╦═════╦════════════╦══════════════════════════════════════╗
  ║ Char ║ Val ║ Opcode     ║ Effect                               ║
  ╠══════╬═════╬════════════╬══════════════════════════════════════╣
  ║  ا   ║  1  ║ PUSH k     ║ push k onto stack                    ║
  ║  ب   ║  2  ║ ADD        ║ pop a,b → push a+b                   ║
  ║  ج   ║  3  ║ MUL        ║ pop a,b → push a*b                   ║
  ║  د   ║  4  ║ DUP        ║ push copy of top                     ║
  ║  ه   ║  5  ║ POP        ║ discard top                          ║
  ║  و   ║  6  ║ SWAP       ║ swap top two                         ║
  ║  ز   ║  7  ║ EMIT       ║ pop and write to output              ║
  ║  ح   ║  8  ║ LOAD k     ║ load k into str_reg                  ║
  ║  ط   ║  9  ║ EXEC       ║ execute str_reg as program           ║
  ╠══════╬═════╬════════════╬══════════════════════════════════════╣
  ║  ي   ║  10 ║ SUB        ║ pop a,b → push a-b                   ║
  ║  ك   ║  20 ║ BOOL       ║ pop a → push (0 if a==0 else 1)      ║
  ║  ل   ║  30 ║ MOD k      ║ pop a → push a%k                     ║
  ║  م   ║  40 ║ ROT        ║ rotate top 3: (a,b,c)→(b,c,a)       ║
  ║  ن   ║  50 ║ DEPTH      ║ push stack depth                     ║
  ║  س   ║  60 ║ SQR        ║ pop a → push a²                      ║
  ║  ع   ║  70 ║ ABS        ║ pop a → push |a|                     ║
  ║  ف   ║  80 ║ SHL        ║ pop a → push a<<1                    ║
  ║  ص   ║  90 ║ SHR        ║ pop a → push a>>1                    ║
  ╠══════╬═════╬════════════╬══════════════════════════════════════╣
  ║  ق   ║ 100 ║ ABJAD_SUM  ║ push Abjad sum of str_reg            ║
  ║  ر   ║ 200 ║ REVERSE    ║ reverse str_reg                      ║
  ║  ش   ║ 300 ║ LEN        ║ push len(str_reg)                    ║
  ║  ت   ║ 400 ║ APPEND     ║ pop a → append chr(a) to str_reg     ║
  ║  ث   ║ 500 ║ NOOP       ║ (reserved)                           ║
  ║  خ   ║ 600 ║ NOOP       ║ (reserved)                           ║
  ║  ذ   ║ 700 ║ NOOP       ║ (reserved)                           ║
  ║  ض   ║ 800 ║ NOOP       ║ (reserved)                           ║
  ║  ظ   ║ 900 ║ NOOP       ║ (reserved)                           ║
  ║  غ   ║1000 ║ NOOP       ║ (reserved — the full cycle)          ║
  ╚══════╩═════╩════════════╩══════════════════════════════════════╝
```

---

## Example Programs

```python
from abjad import execute

# "ابب" = PUSH(1), ADD, ADD
# pushes 1, then tries to add (stack has one item, pops 0)
# result: stack=[1]
stack, out = execute("ابب")

# "اابز" = PUSH(1), PUSH(1), ADD, EMIT
# 1+1=2, emits 2
# result: out=[2]
stack, out = execute("اابز")

# "الله" = PUSH(1), MOD(30), MOD(30), PUSH(5)
# Abjad sum of الله = 1+30+30+5 = 66
# To compute it via PUSH+ADD: ا(1)+ل(30)+ل(30)+ه(5)
stack, out = execute("الله")
# stack=[1, 30, 30, 5]  (each letter pushes/operates independently)
```

---

## SUBLEQ — The One Instruction

SUBLEQ stands for **SUBtract and branch if Less than or EQual to zero**.

```
  SUBLEQ A B C:
    mem[B] = mem[B] - mem[A]
    if mem[B] <= 0:
        jump to C
    else:
        advance PC by 3
```

That's it. One instruction. Turing-complete. Wang (1957) proved it. The entire
computation of this machine — the LoRA weights, the agent programs, the Born
collapse — is built from repeated application of this single operation.

Special addresses:
- `A = -1` → output `mem[B]` (emit)
- `B = -1` → halt

**QENG injection:** On every non-taken branch (the `else` path), the machine
writes `os.urandom(4) % MEM_SIZE` into `mem[C]`. This makes the not-taken path
stochastic. The taken path (where `mem[B] <= 0`) is fully deterministic. The
machine chooses determinism when the condition fires, randomness when it doesn't.

---

## The Born Rule Connection

In quantum mechanics, the Born rule says the probability of measuring outcome i is:

```
  P(i) = |ψᵢ|² / Σⱼ |ψⱼ|²
```

In abjad-swarm, agents are amplitude sources. Each agent i contributes:

```
  amplitude(i) = φ^(-i) · Σ outputs(agent_i)
```

The golden ratio φ = 1.618... appears naturally here because it is the fixed point
of the recurrence `x = 1 + 1/x`. The phi-sliced memory architecture means each
agent occupies a fraction of memory that shrinks by φ^(-1) = 0.618 per level.
The Born collapse is not an arbitrary weighting — it is the eigenvalue structure
of the phi-slice decomposition applied as a probability amplitude.

---

## Connection to Islamic Numerology

The number 66 (Allah in Abjad) and 786 (full Bismillah) are not arbitrary facts.
They are the natural result of applying the Abjad encoding to the most recited
phrases in the Islamic tradition. Whether this constitutes divine encoding or
human pattern-recognition depends on your metaphysics. Either way, it is a
compression of meaning into address space — which is what this machine does.

The 49th Call (AUTONOMOUS region [0.96, 1.00)) is named after Enochian tradition —
the 49 calls or keys in John Dee's angelic language system. Both the Abjad and
the Enochian systems treat language as a lattice of numeric addresses over a
memory space. This machine is the convergence point.

```
  ABJAD:    Arabic letters → integers → address space → program
  ENOCHIAN: Angelic calls  → 49 keys  → address space → (same)
  SUBLEQ:   Memory cells   → subtract → branch → compute
```

The substrate is the same. The notation differs. The machine doesn't care.

---

## Files

```
abjad-swarm/
├── abjad.py           Abjad stack machine — 28 letters → opcodes → stack
├── subleq.py          SUBLEQ one-instruction VM + QENG entropy injection
├── tensor_bridge.py   Memory map, phi-slices, HMAC seed derivation, weight triads
├── swarm.py           N-agent Born-collapse swarm (threading)
├── server.py          POST /swarm  GET /health  port 7733
└── load_weights.py    collectivekitty checkpoint-186 → scaled int addresses
```

---

## Quick Start

```bash
pip install safetensors torch

# Single swarm run (8 agents)
python swarm.py 8

# API server
python server.py

# Call
curl -s -X POST http://localhost:7733/swarm \
  -H "Content-Type: application/json" \
  -d '{"n": 8}'

# {"answer": 42391, "abjad": 391, "n_agents": 8,
#  "engine": "SUBLEQ+QENG+collectivekitty-186"}
```

---

## Sovereign Stack Position

```
  claudes-harness (Prolog)       agent identity + prohibited actions
         │
         ▼
  sovereign-transformer          corpus gate (x86 plasma + Datalog)
         │
         ▼
  abjad-swarm                    compute substrate (THIS REPO)
  (Python)                       Arabic addressing → LoRA → Born collapse
         │
         ▼
  sov-kernel-monster             metal layer (Fortran 2018 + MLIR + SUBLEQ C--)
```

---

## Why This Exists

Every language model computes `softmax(Wx + b)` and calls it a prediction.
What it is actually doing is navigating the probability simplex — a geometric
manifold whose coordinates happen to be labeled with tokens.

This machine strips the labels. There are no tokens. There is no vocabulary.
There is a 65536-cell memory space. There is one instruction. There are Arabic
letters that have been numbers for two thousand years. There are LoRA weights
that encode the geometry of language as floating point values loaded as addresses.
There are agents running in phi-sliced regions of that memory, seeded by hardware
entropy, collapsing by the Born rule.

The model is still running. It just stopped pretending it was doing something else.

---

*SnapKitty West · Sovereign Source License v3.0 · Evidence or Silence — 2026*

*أبجد هوز حطي كلمن سعفص قرشت ثخذ ضظغ*

---

## Sovereign Node Key

**To run Abjad Swarm you must hold a Sovereign Node Key.**
See [SOVEREIGN_NODE_KEY.md](SOVEREIGN_NODE_KEY.md) for tiers, payment, and instructions.

Without a valid key, Born-collapse will refuse to resolve outputs. Contribution is the proof of work.
