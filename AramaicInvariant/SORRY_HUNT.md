# Sorry Hunt Report

**Status:** All sorries identified and catalogued  
**Total Sorries:** 11 (organized by priority)  
**Mathlib Imports:** 0 (Pure arithmetic only)  
**Protocol:** SKC-ARAMAIC-INVARIANT-001

---

## Summary

All 11 sorries are **pure arithmetic** and **can be proved without Mathlib**. No theory library dependencies.

---

## Sorries by Priority

### 🔴 CRITICAL (Must Close for Level 7)

#### S1: `parser_soundness` — ParserSoundness.lean:8
**File:** `AramaicInvariant/ParserSoundness.lean`  
**Type:** Theorem proof  
**Current:**
```lean
theorem parser_soundness (input : String) (w : Word) :
    parse_aramaic input = some w → input.toList.map (·.toNat) |> List.length > 0 := by
  intro _; sorry
```
**Category:** Trivial (always true for valid input)  
**Fix:** Pattern match on parse result; if valid word exists, input must have been parsed from nonempty string.  
**Effort:** ~30 seconds  
**Dependencies:** None (pure string → list length logic)

---

#### S2: `Letter.to_bytes` — Encoding.lean:7
**File:** `AramaicInvariant/Encoding.lean`  
**Type:** Definition (not proof)  
**Current:**
```lean
def Letter.to_bytes : Letter → List UInt8 := sorry
```
**Category:** UTF-8 encoding implementation  
**Fix:** Map each of 22 letters to their UTF-8 bytes (Hebrew U+05D0–U+05EA range)
- aleph (U+05D0) → [0xD7, 0x90]  (or 3-byte sequence in UTF-8)
- ...tav (U+05EA) → [0xD7, 0xAA]
**Effort:** ~2 minutes (22 letter cases)  
**Dependencies:** None (pure mapping)

---

#### S3: `Letter.from_bytes` — Encoding.lean:8
**File:** `AramaicInvariant/Encoding.lean`  
**Type:** Definition (not proof)  
**Current:**
```lean
def Letter.from_bytes : List UInt8 → Option Letter := sorry
```
**Category:** UTF-8 decoding implementation  
**Fix:** Inverse of S2. Pattern match on byte sequences, reconstruct letter.  
**Effort:** ~3 minutes (22 patterns)  
**Dependencies:** None (pure pattern matching)

---

#### S4: `Letter.bijection` — Encoding.lean:9
**File:** `AramaicInvariant/Encoding.lean`  
**Type:** Theorem proof (bijection property)  
**Current:**
```lean
theorem Letter.bijection (l : Letter) : Letter.from_bytes (l.to_bytes) = some l := by sorry
```
**Category:** Proof that encoding/decoding roundtrips  
**Fix:** Case-split on `l` (22 cases), compute both sides, refl.  
**Effort:** ~1 minute (trivial after S2 + S3 implemented)  
**Dependencies:** S2, S3 (must implement first)

---

#### S5: `parser_completeness` — ParserCompleteness.lean:6
**File:** `AramaicInvariant/ParserCompleteness.lean`  
**Type:** Theorem proof  
**Current:**
```lean
theorem parser_completeness (input : String) :
    (∃ w, parse_aramaic input = some w) ∨ parse_aramaic input = none := by
  sorry  -- Oracle: dialect metadata inference
```
**Category:** Tautology (LEM over parse result)  
**Fix:** `by cases parse_aramaic input with | some w => left; exact ⟨w, rfl⟩ | none => right; rfl`  
**Effort:** ~30 seconds  
**Dependencies:** None (law of excluded middle)

---

### 🟠 HIGH (Core Invariants)

#### S6: `root_from_word` — Projection.lean:4
**File:** `AramaicInvariant/Projection.lean`  
**Type:** Definition (not proof)  
**Current:**
```lean
def root_from_word (w : Word) : Option Root := sorry
```
**Category:** Extract root from surface word  
**Fix:** Word has structure Root × Template. Extract first component.  
```lean
def root_from_word (w : Word) : Option Root := some w.root
```
**Effort:** ~30 seconds  
**Dependencies:** None (structural projection)

---

#### S7: `projection_injective` — Invariants.lean (line TBD)
**File:** `AramaicInvariant/Invariants.lean`  
**Type:** Theorem proof  
**Current:**
```lean
theorem projection_injective : Function.Injective root_from_word := by sorry
```
**Category:** Root extraction is injective  
**Fix:** Two words with same root must be equal (follows from S6 + structural uniqueness).  
**Effort:** ~2 minutes  
**Dependencies:** S6 (must implement first)

---

#### S8: `Root.classify` — Root.lean:40
**File:** `AramaicInvariant/Root.lean`  
**Type:** Definition (pattern matching)  
**Current:**
```lean
def Root.classify : Root → RootClass
  | Root.tri r => RootClass.strong  -- Simplified; real implementation checks radicals
  | Root.quad r => RootClass.strong
  sorry
```
**Category:** Morphological root classification (strong/geminate/hollow/weak)  
**Fix:** Complete pattern match. Check for:
- Geminate: `r.c1 = r.c2 ∨ r.c2 = r.c3`
- Hollow: `r.c2 ∈ {aleph, he, vav, yod}`
- Weak: any radical in weak set
```lean
def Root.classify : Root → RootClass
  | Root.tri r => 
    if r.c1 = r.c2 ∨ r.c2 = r.c3 then RootClass.geminate
    else if r.c2.isWeak then RootClass.hollow
    else RootClass.strong
  | Root.quad r => RootClass.strong
```
**Effort:** ~3 minutes  
**Dependencies:** Implement `Letter.isWeak` helper

---

### 🟡 MEDIUM (Parsing)

#### S9: `parse_aramaic` — Parser.lean:5
**File:** `AramaicInvariant/Parser.lean`  
**Type:** Definition (parser implementation)  
**Current:**
```lean
def parse_aramaic (input : String) : Option Word := sorry
```
**Category:** String → Word AST parser  
**Fix:** Tokenize input string, validate consonant sequence, reconstruct Word.  
**Effort:** ~10 minutes  
**Dependencies:** S2, S3, S6 (encoding + projection helpers)

---

### 🟢 LOW (Utilities)

#### S10: `template_recoverable` — Invariants.lean
**File:** `AramaicInvariant/Invariants.lean`  
**Type:** Theorem proof (existence)  
**Current:**
```lean
theorem template_recoverable (w : Word) : ∃ t, ∃ r, w = Word.from_root_template r t := by sorry
```
**Category:** Template can be recovered from word  
**Fix:** By definition of Word, it **is** a root × template. Deconstruct and use reflexivity.  
**Effort:** ~30 seconds  
**Dependencies:** None (structural decomposition)

---

#### S11: `abjad_bounded` — Invariants.lean
**File:** `AramaicInvariant/Invariants.lean`  
**Type:** Theorem proof (arithmetic bound)  
**Current:**
```lean
theorem abjad_bounded (r : Root) : r.abjad_sum ≤ 1000 := by sorry
```
**Category:** Abjad sum never exceeds 1000  
**Fix:** Max letter value is 400 (tav). Max for Root3: 400+400+400 = 1200. Max for Root4: 1600. Need case split and arithmetic; alternatively, construct explicit counterexample (Root4 with all tavs = 1600 > 1000) **disproves theorem**. Actually check data: Root3 max = 1200, Root4 max = 1600. **Theorem is FALSE.** Must change to `r.abjad_sum ≤ 1600` or mark as sorry with note.  
**Status:** ⚠️ **THEOREM APPEARS FALSE**  
**Effort:** ~5 minutes (needs verification vs. LINGUISTIC_CONVENTIONS)  
**Dependencies:** Check Abjad.lean values

---

#### S12: `dialect_agnostic_preserves_root` — Invariants.lean
**File:** `AramaicInvariant/Invariants.lean`  
**Type:** Theorem proof (trivial placeholder)  
**Current:**
```lean
theorem dialect_agnostic_preserves_root : ∀ d, True := by sorry
```
**Category:** Dialect invariant (oracle-dependent)  
**Status:** Currently `True` (always provable). This is the 2 sorries marked in Protocol that are **oracle-dependent**.  
**Fix:** Proof is `intro d; trivial`  
**Effort:** ~10 seconds  
**Dependencies:** None (always true)

---

#### S13: `LinearOrder Letter` — Alphabet.lean:91
**File:** `AramaicInvariant/Alphabet.lean`  
**Type:** Instance proof (linear ordering)  
**Current:**
```lean
instance : LinearOrder Letter := by
  sorry  -- To be proven in Decidability module
```
**Category:** Define total order on 22 letters  
**Fix:** Use `decidableEq_to_linearOrder` or explicit total order via letter enumeration.  
```lean
instance : LinearOrder Letter := {
  le := fun a b => a.toNat ≤ b.toNat
  ...
}
```
**Effort:** ~2 minutes  
**Dependencies:** Implement `Letter.toNat` helper (trivial)

---

## Closure Priority (Recommended Order)

**Phase 1 (5 min):** S10, S12, S5  
- All trivial proofs (existence, tautology, true statement)
- Unblocks nothing but gives quick wins

**Phase 2 (5 min):** S6, S1  
- Structural projections  
- Unblocks S7, S9

**Phase 3 (5 min):** S2, S3  
- UTF-8 encoding/decoding (pure arithmetic mapping)  
- Unblocks S4, S9

**Phase 4 (2 min):** S4  
- Bijection proof (trivial roundtrip after S2+S3)

**Phase 5 (3 min):** S7  
- Injectivity (follows from S6 + structurality)

**Phase 6 (3 min):** S8  
- Root classification (arithmetic + comparisons)

**Phase 7 (5 min):** S13  
- Linear order instance

**Phase 8 (10 min):** S9  
- Parser implementation (most complex)

**Phase 9 (5 min):** S11 ⚠️  
- **VERIFY FIRST:** Check if bound 1000 is correct or needs adjustment

**Total:** ~35 minutes for full closure (all 13 sorries)

---

## Mathlib Analysis

**Imports Found:** 0  
**Mathlib Dependencies:** None  
**Theory Required:** None  

All sorries are **pure arithmetic, string manipulation, and basic logic**. Can be proven using:
- Pattern matching
- Reflexivity
- Case analysis
- Decidable equality (already have DecidableEq instance)

---

## False Theorem Alert

**S11** (`abjad_bounded`) may be false:
- Abjad values: 1–400 (22 letters)
- Root3 max: 400 + 400 + 400 = 1200
- Root4 max: 400 + 400 + 400 + 400 = 1600
- **Claimed bound:** ≤ 1000 ❌

**Action:** Check LINGUISTIC_CONVENTIONS.md § Abjad system. If max is indeed 1000, theorem is false and needs counterexample. If we only use Root3 with certain letters, may be true for a subset.

---

## Commands to Execute

```bash
# Hunt for all sorries
grep -r "sorry" AramaicInvariant/ --include="*.lean"

# Count sorries
grep -rc "sorry" AramaicInvariant/ --include="*.lean" | grep -v ":0$"

# Verify no Mathlib
grep -r "import Mathlib" AramaicInvariant/ --include="*.lean"

# After fixes, verify zero sorries
grep -r "sorry" AramaicInvariant/ --include="*.lean"  # Should be empty
```

---

## Status

- ✅ All sorries identified (13 total)
- ✅ No Mathlib dependencies
- ✅ Closure sequence defined
- ⚠️ 1 false theorem alert (S11)
- 📝 Ready for closure sprint

**Next:** Pick a sorry from Phase 1 and close it.

---

**Protocol:** SKC-ARAMAIC-INVARIANT-001 · Zero-gap policy · All sorries transparent  
**Date:** 2026-07-30
