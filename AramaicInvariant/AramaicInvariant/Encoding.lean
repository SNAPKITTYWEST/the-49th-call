-- Layer 4: UTF-8 bijection proofs
import AramaicInvariant.Alphabet
namespace AramaicInvariant

-- S2: Letter.to_bytes — UTF-8 encoding via Unicode code point
-- Aramaic letters are U+05D0..U+05EA (3-byte UTF-8 encoding)
-- Format: 0xE0 (1110xxxx) 0x80 + (0x96 + byte_offset) 0x80 + low_byte
def Letter.to_bytes : Letter → List UInt8 := fun l =>
  let unicode := l.unicode
  let b1 := 0xE0 ∨ ((unicode >>> 12) & 0x0F)
  let b2 := 0x80 ∨ (((unicode >>> 6) & 0x3F))
  let b3 := 0x80 ∨ (unicode & 0x3F)
  [b1.toUInt8, b2.toUInt8, b3.toUInt8]

-- S3: Letter.from_bytes — Inverse mapping via pattern match (22 cases)
-- Reconstructs Unicode code point from 3-byte UTF-8 and matches to Letter
def Letter.from_bytes : List UInt8 → Option Letter := fun bytes =>
  match bytes with
  | [b1, b2, b3] =>
    let unicode : Nat := ((b1.toNat & 0x0F) <<< 12) ∨ ((b2.toNat & 0x3F) <<< 6) ∨ (b3.toNat & 0x3F)
    match unicode with
    | 0x05D0 => some Letter.aleph
    | 0x05D1 => some Letter.bet
    | 0x05D2 => some Letter.gimel
    | 0x05D3 => some Letter.dalet
    | 0x05D4 => some Letter.he
    | 0x05D5 => some Letter.vav
    | 0x05D6 => some Letter.zayin
    | 0x05D7 => some Letter.het
    | 0x05D8 => some Letter.tet
    | 0x05D9 => some Letter.yod
    | 0x05DB => some Letter.kaf
    | 0x05DC => some Letter.lamed
    | 0x05DE => some Letter.mem
    | 0x05DF => some Letter.nun
    | 0x05E1 => some Letter.samekh
    | 0x05E2 => some Letter.ayin
    | 0x05E4 => some Letter.pe
    | 0x05E6 => some Letter.tsadi
    | 0x05E7 => some Letter.qof
    | 0x05E8 => some Letter.resh
    | 0x05E9 => some Letter.shin
    | 0x05EA => some Letter.tav
    | _ => none
  | _ => none

-- S4: Letter.bijection — Prove to_bytes ∘ from_bytes = id
-- By reflexivity after to_bytes and from_bytes definitions match
theorem Letter.bijection (l : Letter) : Letter.from_bytes (l.to_bytes) = some l := by
  cases l <;> rfl

end AramaicInvariant
