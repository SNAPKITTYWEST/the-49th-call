-- Layer 1: Foundational Types — Root
-- Triconsonantal (3-letter) and quadriliteral (4-letter) roots

import AramaicInvariant.Alphabet

namespace AramaicInvariant

/-- Root class (morphological type) -/
inductive RootClass : Type where
  | strong       -- Strong root (all radicals present, no special rules)
  | geminate     -- Geminate root (two radicals identical)
  | hollow       -- Hollow root (middle radical is w/y)
  | weakDouble   -- Weak-double root (two middle/final radicals weak)
  | weakTriple   -- Weak-triple root (two or more weak radicals)
  | weak         -- General weak root
  deriving DecidableEq, Repr

/-- Triconsonantal root (3 letters) -/
structure Root3 where
  c1 : Letter
  c2 : Letter
  c3 : Letter
  deriving DecidableEq, Repr

/-- Quadriliteral root (4 letters) -/
structure Root4 where
  c1 : Letter
  c2 : Letter
  c3 : Letter
  c4 : Letter
  deriving DecidableEq, Repr

/-- General root type (triconsonantal or quadriliteral) -/
inductive Root : Type where
  | tri (r : Root3) : Root
  | quad (r : Root4) : Root
  deriving DecidableEq, Repr

/-- Classify root by its morphological properties -/
def Root.classify : Root → RootClass
  | Root.tri r =>
    -- Triconsonantal classification
    if r.c1 = r.c2 ∧ r.c2 = r.c3 then
      RootClass.strong  -- All identical (rare, default strong)
    else if r.c1 = r.c2 ∨ r.c2 = r.c3 then
      RootClass.geminate  -- Two consonants identical
    else if r.c2.isWeak then
      RootClass.hollow  -- Middle radical is weak (w/y)
    else
      RootClass.strong  -- All present, no special rules
  | Root.quad r =>
    -- Quadriliteral classification
    if r.c1 = r.c2 ∧ r.c3 = r.c4 then
      RootClass.geminate  -- Double-geminate quad
    else if r.c2.isWeak ∨ r.c3.isWeak then
      RootClass.hollow  -- Hollow quad
    else
      RootClass.strong  -- Strong quad

/-- Extract consonants from root -/
def Root.consonants : Root → List Letter
  | Root.tri r => [r.c1, r.c2, r.c3]
  | Root.quad r => [r.c1, r.c2, r.c3, r.c4]

end AramaicInvariant
