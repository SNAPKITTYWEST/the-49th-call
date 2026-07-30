# Formal Morphological Model for Aramaic Root Extraction

## 1. Core Type Definitions

### 1.1 Root Type

```
Σ ≔ {א, ב, ג, ד, ה, ו, ז, ח, ט, י, כ, ל, מ, נ, ס, ע, פ, צ, ק, ר, ש, ת}
  (Aramaic consonants, excluding weak markers in root position)

Root ≔ Root3(c₁, c₂, c₃) : c₁,c₂,c₃ ∈ Σ ∪ {ø}
     | Root4(c₁, c₂, c₃, c₄) : c₁,c₂,c₃,c₄ ∈ Σ ∪ {ø}

RootClass ≔ Strong | Geminate | Hollow | Weak(position ∈ {I,II,III})
```

**Semantics:**
- **Strong root**: No gutturals (א, ה, ח, ע) or weak consonants (ו, י, ן)
- **Geminate**: c₂ = c₃ (e.g., √שמם "be desolate")
- **Hollow**: c₂ ∈ {ו, י} (e.g., √קום "rise")
- **Weak**: One position contains ו, י, or ה (e.g., √בנה "build", √שתה "drink")

---

### 1.2 Vocalism (Vocalic Template)

```
Vowel ≔ Patach | Segol | Tsere | Hiriq | Holem | Qubbuts | Shwa
      | Long_Patach | Long_Segol | Long_Tsere | Long_Hiriq | Long_Holem
      | Neutral (ə schwa, unmarked)

VocalicTemplate ≔ (C, v₁, C, v₂, C) : v₁,v₂ ∈ Vowel
               | (C, v₁, C, v₂, C, C) : v₁,v₂ ∈ Vowel
               | (v₀, C, v₁, C, v₂, C) : v₀,v₁,v₂ ∈ Vowel (prothetic vowel)

Binyan ≔ Qal
      | Niphal | Piel | Pual
      | Haphel | Shaphel (Aramaic causatives)
      | Ethpeal | Ethpaal (Aramaic passives/reflexives)
      | Qattel, Itpal, Ittapal (Aramaic verb patterns)

BinyanhopTemplate : Binyan → VocalicTemplate
```

**Standard templates by Binyan:**

| Binyan | Pattern | Example: √שׁמר |
|--------|---------|----------------|
| Qal | CaCaC | שׁמר |
| Qal-impf | CiCuC / CaCuC | יִשׁמור / יַשׁמור |
| Niphal | NiCCaC | נִשׁמַר |
| Piel | CiCCeC | שִׁמִּר |
| Pual | CuCCaC | שֻׁמַּר |
| Haphel | HaCCiC | הַשׁמִּר |
| Ethpeal | ItCaCCaC | אִתְשׁמַר |

---

### 1.3 Affix Type

```
AffixType ≔ Prefix | Infix | Suffix

Prefix ≔ PrefixMarker(clitic, morpheme, position)
       where clitic ∈ {ב, ל, כ, ...} (preposition/particle)
       morpheme ∈ {ה-, י-, ת-, נ-, ...} (person/tense/aspect)

Infix ≔ InfixMarker(gemination | doubling | n-insertion)
      (e.g., Niphal נ-, Piel gemination)

Suffix ≔ PersonalSuffix(person, number, gender, attachment)
       | PossessiveSuffix(person, number, gender)
       | StateSuffix(absolute | construct | emphatic)
       | TenseAspectSuffix(perfect : person×number×gender
                         | imperfect : person×number×gender
                         | etc.)

PersonalEnding ≔ (-ת, -ו, -ה, -ן, -ית, -ית, ...) : person × number × gender
```

**Standard prefix combinations:**

```
PrefixString ≔ [Preposition] [Relativizer] [Tense/Aspect] [Person]
              ∈ {ε, ב, ל, כ} × {דִי, ...} × {י-, ת-, נ-, א-} × {ו, ...}

Example: וַיִּשׁמְרוּ = ו (copula) + י (impf 3m) + שׁמר (root) + ו (3mp suffix)
```

---

### 1.4 Inflection

```
Inflection ≔ (tense, aspect, mood, person, number, gender, state)

Tense ≔ Perfect | Imperfect | Participle | Infinitive

Aspect ≔ Perfective | Imperfective | Habitual | Progressive

Mood ≔ Indicative | Jussive | Cohortative | Imperative | Conditional

Person ≔ First | Second | Third

Number ≔ Singular | Plural | (Dual: archaic)

Gender ≔ Masculine | Feminine | (Common: rare)

State ≔ Absolute | Construct | Emphatic (special for nouns/adjectives)
```

---

### 1.5 Surface Word

```
Orthography ≔ Unpointed | FullyPointed | PartiallyPointed

Script ≔ Aramaic_Square (standard)
       | Aramaic_Cursive (historical)
       | Hebrew_Square (later overlay)

Dialect ≔ Biblical | Official | Jewish_Aramaic | Christian | Syriac_Influence

SurfaceWord ≔ (chars : List Char,
              orthography : Orthography,
              script : Script,
              dialect : Dialect,
              vocalPoints : Option (List VocalPoint),
              stateMarker : Option StateMarker,
              metadata : Dict String String)

VocalPoint ≔ (position : Nat, vowel : Vowel)

StateMarker ≔ (Construct_Marker | Emphatic_Marker)
```

**Example surface forms:**
- `שׁמר` (unpointed, Absolute)
- `שִׁמְרוּ` (pointed, masculine plural 3rd, Perfect)
- `יִשׁמְרוּן` (pointed, masculine plural 3rd, Imperfect Jussive)

---

## 2. Morphological Operations

### 2.1 Root Extraction (Parsing Relation)

**Relational definition (before optimization):**

```
ParseMorpheme(surfaceWord : SurfaceWord) ⊆ Root × Binyan × Affix × Inflection

ParseMorpheme(word) ≔ {(r, b, a, i) | 
    ∃ roots = CandidateRoots(chars(word)),
    ∃ b ∈ Binyan,
    ∃ a ∈ ValidAffixes(b, i),
    ∃ i ∈ ValidInflections(b),
    Derive(r, b, a, i) ≈ word
}

where Derive : Root × Binyan × Affix × Inflection → SurfaceWord
      ≈ denotes surface similarity (accounting for orthographic variation)
```

---

### 2.2 Morphological Derivation Function

```
Derive : Root × Binyan × Affix × Inflection → SurfaceWord

Derive(root, binyan, affix, infl) ≔
  let template = BinyanhopTemplate(binyan)
  let rootCons = Expand(root, RootClass(root))    -- expand weak positions
  let vocalized = ApplyTemplate(template, rootCons, infl)
  let affixed = ApplyAffixes(vocalized, affix, infl)
  let pointed = AddVocalPoints(affixed, infl, binyan)
  let orthographized = NormalizeOrthography(pointed, dialect, script)
  in orthographized
```

**Subfunction: Expand**

```
Expand : Root → RootClass → List Char

-- Strong root: identity
Expand(Root3(c₁, c₂, c₃), Strong) = [c₁, c₂, c₃]

-- Geminate: expand c₂ = c₃ if needed for certain binyanim
Expand(Root3(c₁, c₂, c₂), Geminate) =
  case binyan of
  | Piel | Haphel => [c₁, c₂, c₂]  -- gemination preserved/strengthened
  | Qal => [c₁, c₂, c₂]
  | _ => [c₁, c₂, c₂]

-- Hollow: interpolate omitted second radical (often ו, י)
Expand(Root3(c₁, ø, c₃), Hollow(II)) =
  case (tense, vowel_pattern) of
  | (Imperfect, e-pattern) => [c₁, י, c₃]
  | (Imperfect, o-pattern) => [c₁, ו, c₃]
  | _ => [c₁, ø, c₃]

-- Weak III (ה, ו):
Expand(Root3(c₁, c₂, ה), Weak(III)) = [c₁, c₂, ה]
-- inflection will handle contraction
```

**Subfunction: ApplyTemplate**

```
ApplyTemplate : VocalicTemplate × List Char × Inflection → SurfaceWord

ApplyTemplate((C, v₁, C, v₂, C), [c₁, c₂, c₃], infl) ≔
  let v₁' = SelectVowel(v₁, infl, binyan)
  let v₂' = SelectVowel(v₂, infl, binyan)
  in [c₁, v₁', c₂, v₂', c₃]
```

---

### 2.3 Root Projection (π_R)

```
π_R : SurfaceWord → Finset Root

π_R(word) ≔ {root : Root |
  ∃ (root, _, _, _) ∈ ParseMorpheme(word)
}
```

**Algorithm (efficient):**

1. **Strip affixes** (prefix, suffix) via recognizer automaton
2. **Extract consonant skeleton** from remaining chars
3. **Query root lexicon** for 3-consonant or 4-consonant matches
4. **Filter by class compatibility** (Geminate, Hollow, Weak)
5. **Return Finset** of candidate roots

```
π_R_efficient(word : SurfaceWord) : Finset Root ≔

  -- Step 1: Remove affixes
  stripped ← StripAffixes(word.chars, word.orthography)

  -- Step 2: Extract consonant skeleton
  consSkeletonCand ← ExtractConsonants(stripped)

  -- Step 3: Query lexicon (Trie-indexed)
  candidates ← RootLexicon.query(consSkeletonCand)

  -- Step 4: Verify via derivation (check if Derive(r, _, _, _) ≈ word)
  verified ← {r ∈ candidates | ∃ (r, _, _, _) ∈ ParseMorpheme(word)}

  return verified
```

---

### 2.4 Root Normalization (N_R)

```
N_R : Root → CanonicalRoot

CanonicalRoot ≔ (radicals : List Σ, 
                 class : RootClass,
                 binyanPref : List Binyan,
                 semanticGloss : String)

N_R(root : Root) ≔
  let radicals = Radicals(root)
  let cls = ClassifyRoot(radicals)
  let prefs = FrequentBinyanim(radicals)
  let gloss = LookupSemantic(radicals)
  in (radicals, cls, prefs, gloss)
```

**Invariants:**

- `N_R ∘ N_R = N_R` (idempotent: normalization commutes with itself)
- For any `word` with `roots = π_R(word)`, all `r ∈ roots` have the same `N_R(r).radicals`
- Canonical form is deterministic and suitable for lexicon indexing

---

## 3. Parsing Relation (Formal Specification)

### 3.1 Bidirectional Grammar

**Analysis (SurfaceWord → Morpheme):**

```
Analysis ≔ Lexical(root : Root, binyan : Binyan)
        | Affixed(stem : Lexical, prefix : Prefix, suffix : Suffix)
        | Inflected(morpheme : Morpheme, inflection : Inflection)

Parse : SurfaceWord → Finset Analysis

Parse(word) ≔ FixpointAnalyze(word, RootLexicon, AffixLexicon)
```

**Synthesis (Morpheme → SurfaceWord):**

```
Synthesize : Analysis × Inflection → SurfaceWord

Synthesize(Inflected(Affixed(Lexical(r, b), pfx, sfx), infl)) ≔
  Derive(r, b, (pfx, sfx), infl)
```

---

### 3.2 Morphological Grammar in BNF

```
⟨Word⟩ ::= ⟨Proclitic⟩ ⟨Stem⟩ ⟨Enclitic⟩

⟨Proclitic⟩ ::= ε
              | "ב" | "ל" | "כ" | "מ" | "עַל" | ...

⟨Stem⟩ ::= ⟨BaseMorpheme⟩ ⟨PersonalMarker⟩

⟨BaseMorpheme⟩ ::= ⟨Prefix⟩ ⟨RootInflected⟩ ⟨Suffix_State⟩

⟨Prefix⟩ ::= ε | "ה" | "י" | "ת" | "נ" | "א"

⟨RootInflected⟩ ::= ⟨Root3_Vocalized⟩ | ⟨Root4_Vocalized⟩

⟨Root3_Vocalized⟩ ::= ⟨C⟩ ⟨Vowel⟩ ⟨C⟩ ⟨Vowel⟩ ⟨C⟩

⟨PersonalMarker⟩ ::= ε | "ו" | "ה" | "ן" | "ית" | "ן" | "ותון"

⟨Enclitic⟩ ::= ε | "ה" | "ית" | ⟨PersonalPronoun⟩

⟨PersonalPronoun⟩ ::= "י" | "ך" | "ה" | "ו" | "ה" | "כון" | "הון"
```

---

## 4. Orthographic Variation

### 4.1 Unicode Encoding Schemes

```
EncodingScheme ≔ Hebrew_Square
               | Hebrew_Precomposed (combining diacritics)
               | Aramaic_Paleo_Reconstructed
               | SIL_Transliteration (ASCII equivalent)

VocalPoint_Unicode ≔
  U+05B0 (Shwa) | U+05B1 (Hatef_Patach) | U+05B2 (Hatef_Segol)
  | U+05B3 (Hatef_Qubbuts) | U+05B4 (Hiriq) | U+05B5 (Tsere)
  | U+05B6 (Segol) | U+05B7 (Patach) | U+05B8 (Qubbuts)
  | U+05B9 (Holam) | U+05BA (Holam_Haser) | U+05BB (Qubbuts)
  | ...

Normalize_Unicode : SurfaceWord → SurfaceWord (NFD → NFC)
```

### 4.2 Construct State & Emphasis

```
StateMarking ≔ Absolute : "שׁמר"
             | Construct : "שׁמִר" (vowel shortening, often)
                         | "שׁ'מר" (marker variant)
             | Emphatic : "שׁמַּר" (gemination or pointing)
                        | "שׁ:מַר" (marker variant, rare)

StateMarker_Apply : Word × State → Word
```

**Rules:**

- **Absolute** (default): No alteration beyond orthography
- **Construct**: Reduce final vowel (Patach → Schwa), often lose gemination
- **Emphatic**: Add gemination or specific diacritics (Aramaic/Syriac only)

---

### 4.3 Dialect-Specific Orthography

```
DialectOrtho ≔
  | Biblical_Aramaic
    { vowel_full : true,
      state_explicit : true,
      elision_rate : 0.1 }
  
  | Official_Aramaic
    { vowel_full : true,
      state_subtle : true,
      elision_rate : 0.2 }
  
  | Jewish_Aramaic
    { vowel_full : false,
      state_construct_only : true,
      elision_rate : 0.4 }
  
  | Syriac_Christian
    { vowel_diacritical : true,  -- dots, not full vowels
      state_emphatic : true,
      elision_rate : 0.5 }

TransliterateToDialect : SurfaceWord × Dialect → SurfaceWord
```

---

## 5. Complete Morphological Derivation Example

### Example: √שׁמר (watch, keep)

```
Root ≔ Root3(ש, מ, ר), Class = Strong

Binyan = Qal, Perfect, 3rd masculine plural:

Step 1: Expand
  Expand(Root3(ש, מ, ר), Strong) = [ש, מ, ר]

Step 2: Template + Vocalization
  Template(Qal, Perfect, 3mp) = (C, a, C, ø, C)
  Vocalize([ש, מ, ר]) = [ש, a, מ, ø, ר] = "שׁמר"

Step 3: Apply Suffix (3mp personal marker)
  Suffix(3mp) = "ו"
  Result: "שׁמרו" 

Step 4: Normalize Orthography
  FullyPointed + VocalPoints:
    P1: (0, Shin)  -- consonant
    P2: (1, Schwa) -- position 1
    P3: (2, Mem)   -- consonant
    P4: (3, Schwa) -- position 2
    P5: (4, Resh)  -- consonant
    P6: (5, Vav)   -- personal suffix
  Result: שִׁמְרוּ

Analysis = Inflected(
  Affixed(
    Lexical(Root3(ש,מ,ר), Qal),
    ε,  -- no prefix
    "ו" -- 3mp suffix
  ),
  (Perfect, Perfective, Indicative, Third, Plural, Masculine, Absolute)
)

SurfaceWord ≔ {
  chars = ['ש','ִ','מ','ְ','ר','ו','ּ'],
  orthography = FullyPointed,
  script = Hebrew_Square,
  dialect = Biblical_Aramaic,
  vocalPoints = [(1, Hiriq), (3, Schwa), (5, Vav+Dagesh)],
  stateMarker = None,
  metadata = {"binyan": "Qal", "person": "3mp", "tense": "perfect"}
}
```

---

## 6. Inverse Parsing

### 6.1 Lexical Recognizers

```
-- Affix Recognizer (DFA)
AffixRecognizer ≔ {
  start → [ב|ל|כ]? → [ה|י|ת|נ|א]? → stem → [ו|ה|ן|ית]* → accept
}

-- Root Consonant Extractor
ExtractConsonants : SurfaceWord → List Σ
  (strips vowels, diacritics, affixes)

-- Lemmatization Chain
Lemmatize(word) ≔ 
  1. Strip proclitic (ב, ל, כ, מ, ...)
  2. Extract personal suffix (ו, ה, ן, ית, כון, ...)
  3. Identify binyan via remaining shape
  4. Recover root from pattern + consonants
  5. Return (root, binyan, suffixes)
```

---

## 7. Formal Constraints & Theorems

### 7.1 Parsing Soundness

**Theorem 1: Derivation-Parsing Consistency**

```
∀ (r : Root, b : Binyan, a : Affix, i : Inflection):
  π_R(Derive(r, b, a, i)) ∋ r
```

*Proof sketch:* By construction, `Derive` expands radicals and applies templates
such that consonant skeleton matches `r`. The projection `π_R` reverses this;
thus the root is recoverable.

---

### 7.2 Normalization Idempotence

**Theorem 2: N_R Idempotence**

```
∀ r : Root: N_R(N_R(r)) = N_R(r)
```

*Proof:* Canonical roots are unique by radicals + class. Re-normalizing
produces identical radicals, class, and gloss.

---

### 7.3 Weak & Geminate Handling

**Theorem 3: Weak Class Closure**

```
∀ w : Root, Weak(pos): 
  ∃ b : Binyan, π_R(Derive(w, b, ε, infl)) = {w}
  (roots recoverable even with weak positions)
```

*Proof:* Weak expansion rules and orthographic preservation ensure
the degraded radical can be reconstructed from surface form and binyan.

---

## 8. Implementation Roadmap

### Stage 1: Core Types & Lexicon
- [ ] Root, Binyan, Affix, Inflection datatypes
- [ ] Canonical lexicon (1,000+ roots, Lexicon.db)
- [ ] Affixes DFA (prefix/suffix recognizer)

### Stage 2: Derivation
- [ ] Expand (handles Geminate, Hollow, Weak)
- [ ] ApplyTemplate (vocalization by binyan + tense)
- [ ] ApplyAffixes (person, number, gender, state)

### Stage 3: Parsing & Projection
- [ ] ExtractConsonants (consonant skeleton recovery)
- [ ] StripAffixes (prefix/suffix stripping)
- [ ] π_R (candidate root set)
- [ ] Verify via Derive

### Stage 4: Orthographic Variation
- [ ] Unicode normalization (NFD ↔ NFC)
- [ ] State marking (Absolute, Construct, Emphatic)
- [ ] Dialect transliteration (Biblical, Official, Jewish, Syriac)

### Stage 5: Formal Verification
- [ ] Prove Theorem 1 (Derivation-Parsing Consistency)
- [ ] Prove Theorem 2 (N_R Idempotence)
- [ ] Prove Theorem 3 (Weak Class Closure)

---

## References & Notes

**Linguistic precedent:**
- Goldenberg, G. (1998). *Aramaic Phonology.* In *Phonologies of Asia and Africa*.
- Barth, F. (1894). *Grammatik und Orthographie des aramäischen*.
- Rosenthal, F. (1961). *A Grammar of Biblical Aramaic*, 2nd ed.

**Formal morphology:**
- Kaplan & Kay (1994). *Regular models of phonological rule systems.*
- Beesley & Karttunen (2003). *Finite-State Morphology.*

**Implementation libraries:**
- FOMA (finite-state toolkit)
- Petrie (constraint-based morphology, Lean4)
- Custom Trie-based root lexicon (optimized for Aramaic)

---

**Document Status:** Formal specification, Stage 1-2 implementation ready.

**Next action:** Parse Hebrew corpus sample, validate against existing tagged data.
