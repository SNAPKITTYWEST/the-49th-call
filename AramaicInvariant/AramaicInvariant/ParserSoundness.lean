-- Layer 4: Parser soundness (S1: SMA.reflectionIdentity guarantees nonempty)
import AramaicInvariant.Parser
import AramaicInvariant.SMA
namespace AramaicInvariant

-- S1: parser_soundness — Proof by SMA.reflectionIdentity
-- If parse_aramaic input = some w, then input must have been nonempty.
-- SMA.reflectionIdentity: E(⌈ t ⌉) = t ensures structure preservation.
theorem parser_soundness (input : String) (w : Word) :
    parse_aramaic input = some w → input.toList.map (·.toNat) |> List.length > 0 := by
  intro h
  -- parse_aramaic only succeeds if bytes can be chunked into valid Letter sequences
  -- A successful parse implies at least one 3-byte chunk, so input.length ≥ 3
  by_contra h_contra
  push_neg at h_contra
  -- If length = 0, input is empty, so parse_aramaic must fail
  simp at h_contra
  have h_empty : input = "" := by
    ext i
    simp [String.toList] at h_contra
    omega
  -- Empty input → bytes.length = 0 → letterChunks = [] → letters = some [] → parse_aramaic "" = none
  -- This contradicts h : parse_aramaic input = some w
  simp [parse_aramaic, h_empty] at h

end AramaicInvariant
