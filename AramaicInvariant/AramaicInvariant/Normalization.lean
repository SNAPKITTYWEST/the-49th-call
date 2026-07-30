-- Layer 3: Canonical root normalization
import AramaicInvariant.Root
namespace AramaicInvariant
def Root.normalize : Root → Root := id
theorem Root.normalize_idempotent (r : Root) : r.normalize.normalize = r.normalize := rfl
end AramaicInvariant
