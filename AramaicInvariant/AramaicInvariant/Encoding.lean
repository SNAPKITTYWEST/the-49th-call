-- Layer 4: UTF-8 bijection proofs
import AramaicInvariant.Alphabet
namespace AramaicInvariant
def Letter.to_bytes : Letter → List UInt8 := sorry
def Letter.from_bytes : List UInt8 → Option Letter := sorry
theorem Letter.bijection (l : Letter) : Letter.from_bytes (l.to_bytes) = some l := by sorry
end AramaicInvariant
