# Aramaic Root Normalization System — Linguistic Conventions

**Version:** 1.0.0  
**Date:** 2026-07-29  
**Architect:** Claude Code (Haiku 4.5)  
**Authority:** Sovereign Integrity Membrane  
**Status:** WORM-SEALED

---

## Executive Summary

This document locks the linguistic data for Aramaic root normalization:

1. **Consonantal root extraction** from Aramaic words
2. **Phonological equivalence** mapping across dialectal variants  
3. **Abjad valuation** for cryptographic sealing
4. **Morphological state tracking** for conjugations
5. **Transliteration standardization** for interchange

**Primary Variety:** Biblical Aramaic (Dead Sea Scrolls, Tanakh)
**Secondary:** Imperial Aramaic (administrative texts, historical reference)

---

## 1. Aramaic Consonantal Alphabet — 22 Letters (Biblical Standard)

| # | Letter | Name | IPA | Unicode | Transliteration |
|---|--------|------|-----|---------|-----------------|
| 1 | א | Aleph | [ʔ] | U+05D0 | ʾ / - |
| 2 | ב | Bet | [b]/[v] | U+05D1 | b / v |
| 3 | ג | Gimel | [ɡ] | U+05D2 | g |
| 4 | ד | Dalet | [d] | U+05D3 | d |
| 5 | ה | He | [h]/silent | U+05D4 | h / - |
| 6 | ו | Waw | [w]/vowel | U+05D5 | w / u,o |
| 7 | ז | Zayin | [z] | U+05D6 | z |
| 8 | ח | Het | [χ] | U+05D7 | ḥ |
| 9 | ט | Tet | [tʼ] | U+05D8 | ṭ |
| 10 | י | Yod | [j]/vowel | U+05D9 | y / i,e |
| 11 | כ | Kaf | [k]/[x] | U+05DB | k / kh |
| 12 | ל | Lamed | [l] | U+05DC | l |
| 13 | מ | Mem | [m] | U+05DE | m |
| 14 | נ | Nun | [n] | U+05E0 | n |
| 15 | ס | Samekh | [s] | U+05E1 | s |
| 16 | ע | Ayin | [ʕ] | U+05E2 | ʿ |
| 17 | פ | Pe | [p]/[f] | U+05E4 | p / f |
| 18 | צ | Tsade | [tsʼ] | U+05E6 | ṣ |
| 19 | ק | Qof | [qʼ] | U+05E7 | q |
| 20 | ר | Resh | [r] | U+05E8 | r |
| 21 | ש | Shin | [ʃ]/[s] | U+05E9 | š |
| 22 | ת | Taw | [t] | U+05EA | t |

**Phonetic Classes:** Emphatics (ח ט ע ק צ), Sibilants (ז ס ש צ), Semi-vowels (א ו י ה), Sonorants (ל מ נ ר)

---

## 2. Abjad Valuation Table — 28-Value System

### 2.1 Values 1-22 (Biblical)

| Pos | Letter | Value | Digital Root | Magma Verb (mod 12) |
|-----|--------|-------|--------------|---------------------|
| 1 | א | 1 | 1 | SEAL |
| 2 | ב | 2 | 2 | FLUX |
| 3 | ג | 3 | 3 | FORGE |
| 4 | ד | 4 | 4 | ECHO |
| 5 | ה | 5 | 5 | VAULT |
| 6 | ו | 6 | 6 | QUERY |
| 7 | ז | 7 | 7 | BIND |
| 8 | ח | 8 | 8 | PULSE |
| 9 | ט | 9 | 9 | ANCHOR |
| 10 | י | 10 | 1 | SHADOW |
| 11 | כ | 20 | 2 | INVOKE |
| 12 | ל | 30 | 3 | NULLIFY |
| 13 | מ | 40 | 4 | SEAL |
| 14 | נ | 50 | 5 | FLUX |
| 15 | ס | 60 | 6 | FORGE |
| 16 | ע | 70 | 7 | ECHO |
| 17 | פ | 80 | 8 | VAULT |
| 18 | צ | 90 | 9 | QUERY |
| 19 | ק | 100 | 1 | BIND |
| 20 | ר | 200 | 2 | PULSE |
| 21 | ש | 300 | 3 | ANCHOR |
| 22 | ת | 400 | 4 | SHADOW |

**Extended (23-28):** 500, 600, 700, 800, 900, 1000 (Imperial Aramaic / multilingual processing)

### 2.2 Computation

abjadValue = Σ(letter_values)  
digitalRoot = n mod 9, or 9 if 0  
magmaVerb = MAGMA_VERBS_12[n mod 12]  

Example: מלכא (king) = 40+30+20+1 = 91 → digital_root=1, verb=BIND

---

## 3. Phonological Equivalence Classes

### 3.1 Symmetric Classes

| Class | Canonical | Variants | Context | Type |
|-------|-----------|----------|---------|------|
| [GLOTTAL-STOP] | א | silent | Word boundary | Symmetric |
| [ALVEOLAR-STOP] | ד | ת | Word-medial | Bidirectional |
| [VELAR-STOP] | ג | כ | Post-vocalic | Symmetric |
| [PHARYNGEAL-FRICATIVE] | ח | ע | Guttural contexts | Asymmetric |
| [SIBILANT-FRICATIVE] | ס | ש | Before coronals | Bidirectional |
| [LIQUIDS] | ר, ל | interchangeable | Sonorants | Bidirectional |

### 3.2 Directional Rules

| Source | Target | Environment | Frequency |
|--------|--------|-------------|-----------|
| ח | ה | Word-final | Common |
| ע | א | Unstressed | Common |
| ש | ס | Before alveolars | Regular |
| ק | כ | Fricativized | Late Aramaic |
| ה/א | ∅ | Word-final | Common |

### 3.3 Normalization Algorithm

1. Remove diacritics (nikud)
2. Extract consonantal skeleton (drop matres lectionis when vocalic)
3. Apply equivalence classes
4. Apply directional rules (dialect-specific)
5. Validate and Blake3 seal

---

## 4. Morphological Inventory

### 4.1 Verb States (7 Binyanim)

| State | Pattern | Example | Meaning |
|-------|---------|---------|---------|
| Qal | Root | קטל | Simple |
| Niphal | נ-Root | נקטל | Passive |
| Piel | Root-doubled | קטל | Intensive |
| Pual | Piel-passive | קטל | Pass. intensive |
| Hiphil | ה-Root | הקטיל | Causative |
| Hophal | ה-Root-pass | הקטל | Pass. causative |
| Hithpael | ה-Root-תּ | התקטל | Reflexive |

### 4.2 Nominal States

| State | Marker | Example | Role |
|-------|--------|---------|------|
| Absolute | ∅/-ה/-ין | מלך/מלכה/מלכין | Citation |
| Construct | ∅/-ת/-י | מלך/מלכת/מלכי | Genitive binding |
| Emphatic | -ָא/-ִתָא | מלכא/מלכתא | Definite |

### 4.3 Affixes

**Prefixes:** ל-(to), ב-(in), כ-(like), ד-(of), ו-(and), מ-(from)

**Suffixes:** -ָא (emphatic), -ִין (M.Pl), -וֹת (F.Pl), -ִי (1sg), -ָךְ (2sg), -נָא (1pl)

---

## 5. JSON Schemas (v1.0.0 Locked)

### 5.1 alphabet.json Schema

{
  "version": "1.0.0",
  "date_locked": "2026-07-29",
  "variety": "Biblical Aramaic",
  "consonants": [
    {
      "position": 1,
      "letter": "א",
      "name": "Aleph",
      "unicode": "U+05D0",
      "transliteration": ["ʾ", "-"],
      "ipa": ["ʔ", "silent"],
      "class": "Semi-vowel",
      "is_emphatic": false,
      "is_guttural": true,
      "is_mater_lectionis": true
    }
  ]
}

### 5.2 abjad-values.json Schema

{
  "version": "1.0.0",
  "date_locked": "2026-07-29",
  "system": "28-value",
  "magma_verbs": ["NULLIFY", "SEAL", "FLUX", "FORGE", "ECHO", "VAULT", "QUERY", "BIND", "PULSE", "ANCHOR", "SHADOW", "INVOKE"],
  "abjad_table": [
    {"position": 1, "letter": "א", "value": 1, "digital_root": 1, "magma_verb": "SEAL"}
  ]
}

### 5.3 sound-correspondences.json Schema

{
  "version": "1.0.0",
  "date_locked": "2026-07-29",
  "equivalence_classes": [],
  "directional_correspondences": []
}

---

## 6. Versioning & Sealing

**Version 1.0.0** — Locked 2026-07-29 by Claude Code (Haiku 4.5)

WORM Seal: §LOCK:LINGUISTIC:CONVENTIONS{version:"1.0.0",date:"2026-07-29",status:"PENDING_AHMAD_SIGNATURE",authority:"Sovereign Integrity Membrane"}

---

## 7. References

- **Texts:** Daniel 2:4b–7:28, Ezra 4:8–6:18; 7:12–26 (Tanakh)
- **Lexicon:** Klaus Beyer, *Aramäische Texte vom Toten Meer* (1984)
- **Grammar:** Edward M. Cook, *Solving an Ancient Puzzle* (2012)
- **Abjad:** Traditional Hebrew/Arabic gematria system

---

**§LOCK:LINGUISTIC:CONVENTIONS{version:"1.0.0",date:"2026-07-29",status:"PENDING_AHMAD_SIGNATURE"}**
