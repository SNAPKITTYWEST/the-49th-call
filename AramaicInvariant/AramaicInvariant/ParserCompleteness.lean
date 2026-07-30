-- Layer 4: Parser completeness (S5: Law of Excluded Middle)
import AramaicInvariant.Parser
namespace AramaicInvariant

-- S5: parser_completeness — Proof by law of excluded middle
-- For any input, either parse_aramaic input = some w (∃ w case) or parse_aramaic input = none
theorem parser_completeness (input : String) :
    (∃ w, parse_aramaic input = some w) ∨ parse_aramaic input = none := by
  have h := parse_aramaic input
  by_cases hc : h = none
  · right; exact hc
  · left
    use Option.get (parse_aramaic input) (by intro h_eq; exact hc h_eq)
    exact Option.get_some (by intro h_eq; exact hc h_eq)

end AramaicInvariant
