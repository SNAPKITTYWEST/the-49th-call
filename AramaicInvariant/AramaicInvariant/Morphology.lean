-- Layer 2: Morphological composition (Root + Template → Word)
import AramaicInvariant.Template
namespace AramaicInvariant
inductive Word : Type where
  | from_root_template (root : Root) (template : VocalicTemplate)
  deriving DecidableEq, Repr
end AramaicInvariant
