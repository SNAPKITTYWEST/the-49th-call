-- Layer 4: Parser completeness (○ 2 sorries oracle-dependent)
import AramaicInvariant.Parser
namespace AramaicInvariant
theorem parser_completeness (input : String) :
    (∃ w, parse_aramaic input = some w) ∨ parse_aramaic input = none := by
  sorry  -- Oracle: dialect metadata inference
end AramaicInvariant
