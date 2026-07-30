-- Layer 3: Dialect variants (Biblical, Imperial, Palestinian)
import AramaicInvariant.Alphabet
namespace AramaicInvariant
inductive Dialect : Type where
  | biblical | imperial | palestinian
  deriving DecidableEq, Repr
end AramaicInvariant
