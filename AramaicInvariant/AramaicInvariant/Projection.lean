-- Layer 3: Root projection (SurfaceWord → Root extraction)
import AramaicInvariant.Morphology
namespace AramaicInvariant

-- S6: root_from_word — Trivial structural projection from Word to Root
-- Word is constructed as Word.from_root_template(root, template),
-- so extraction is pure pattern matching.
def root_from_word (w : Word) : Option Root :=
  match w with
  | Word.from_root_template root _ => some root

end AramaicInvariant
