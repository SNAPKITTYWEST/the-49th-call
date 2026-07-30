-- Layer 2: Morphological Templates (vowel patterns + binyan)
import AramaicInvariant.Root
namespace AramaicInvariant
inductive Binyan : Type where
  | qal | niphal | piel | pual | hiphil | hophal | hithpael
  deriving DecidableEq, Repr
structure VocalicTemplate where
  binyan : Binyan
  pattern : String
  deriving DecidableEq, Repr
end AramaicInvariant
