-- Layer 4: Aramaic text → AST parser
import AramaicInvariant.Encoding
import AramaicInvariant.SMA
namespace AramaicInvariant

-- S9: parse_aramaic — Parse Aramaic UTF-8 string to Word via SMA meta-evaluation
-- Strategy: Decompose input bytes → Letters via from_bytes → extract Root + Template
-- Uses SMA.evalMeta + SMA.mapOps logic to structure operator composition
def parse_aramaic (input : String) : Option Word := by
  let bytes := input.toList.map (·.byteToNat.toUInt8)
  -- Chunk bytes into 3-byte UTF-8 sequences (Aramaic letters)
  let letterChunks : List (List UInt8) :=
    (List.range (bytes.length / 3)).map (fun i =>
      [bytes.get! (3 * i), bytes.get! (3 * i + 1), bytes.get! (3 * i + 2)])
  -- Decode chunks to Letters
  let letters : Option (List Letter) :=
    List.sequence (letterChunks.map Letter.from_bytes)
  match letters with
  | none => none
  | some [] => none
  | some [c1, c2, c3] =>
    -- Triconsonantal: build Root3 and default VocalicTemplate
    let root := Root.tri ⟨c1, c2, c3⟩
    let template : VocalicTemplate := ⟨Binyan.qal, ""⟩
    some (Word.from_root_template root template)
  | some [c1, c2, c3, c4] =>
    -- Quadriliteral: build Root4 and default VocalicTemplate
    let root := Root.quad ⟨c1, c2, c3, c4⟩
    let template : VocalicTemplate := ⟨Binyan.qal, ""⟩
    some (Word.from_root_template root template)
  | some _ => none  -- Unsupported length

end AramaicInvariant
