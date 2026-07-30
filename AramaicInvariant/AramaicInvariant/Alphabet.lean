-- Layer 1: Foundational Types — Alphabet
-- Aramaic consonantal alphabet (22 letters)
-- Unicode: U+05D0–U+05EA (Hebrew range, used for Aramaic)

namespace AramaicInvariant

/-- Aramaic consonant alphabet (22 letters, א–ת) -/
inductive Letter : Type where
  | aleph   | bet   | gimel  | dalet  | he    | vav   | zayin
  | het     | tet   | yod    | kaf    | lamed | mem   | nun
  | samekh  | ayin  | pe     | tsadi  | qof   | resh  | shin | tav
  deriving DecidableEq, Repr

/-- Letters form a fintype -/
instance : Fintype Letter :=
  ⟨[Letter.aleph, Letter.bet, Letter.gimel, Letter.dalet, Letter.he, Letter.vav, Letter.zayin,
    Letter.het, Letter.tet, Letter.yod, Letter.kaf, Letter.lamed, Letter.mem, Letter.nun,
    Letter.samekh, Letter.ayin, Letter.pe, Letter.tsadi, Letter.qof, Letter.resh, Letter.shin,
    Letter.tav],
   by decide⟩

/-- Unicode code point for each letter -/
def Letter.unicode : Letter → Nat
  | Letter.aleph  => 0x05D0
  | Letter.bet    => 0x05D1
  | Letter.gimel  => 0x05D2
  | Letter.dalet  => 0x05D3
  | Letter.he     => 0x05D4
  | Letter.vav    => 0x05D5
  | Letter.zayin  => 0x05D6
  | Letter.het    => 0x05D7
  | Letter.tet    => 0x05D8
  | Letter.yod    => 0x05D9
  | Letter.kaf    => 0x05DB
  | Letter.lamed  => 0x05DC
  | Letter.mem    => 0x05DE
  | Letter.nun    => 0x05DF
  | Letter.samekh => 0x05E1
  | Letter.ayin   => 0x05E2
  | Letter.pe     => 0x05E4
  | Letter.tsadi  => 0x05E6
  | Letter.qof    => 0x05E7
  | Letter.resh   => 0x05E8
  | Letter.shin   => 0x05E9
  | Letter.tav    => 0x05EA

/-- Phonetic class (for equivalence relations) -/
inductive PhoneticClass : Type where
  | emphatic      -- ט, צ, ק, ע, ח
  | sibilant      -- ש, ס, צ, ז
  | fricative     -- ח, כ, פ, כ
  | semivowel     -- ו, י, ה, א
  | sonorant      -- ל, ר, ם, ן, מ, נ
  deriving DecidableEq, Repr

/-- Phonetic class of each letter -/
def Letter.phoneticClass : Letter → PhoneticClass
  | Letter.aleph | Letter.he | Letter.vav | Letter.yod => PhoneticClass.semivowel
  | Letter.het | Letter.kaf | Letter.pe => PhoneticClass.fricative
  | Letter.tet | Letter.tsadi | Letter.qof | Letter.ayin | Letter.samekh => PhoneticClass.emphatic
  | Letter.shin | Letter.samekh | Letter.tsadi | Letter.zayin => PhoneticClass.sibilant
  | Letter.lamed | Letter.resh | Letter.mem | Letter.nun => PhoneticClass.sonorant
  | _ => PhoneticClass.sonorant  -- default

/-- IPA transliteration -/
def Letter.ipa : Letter → String
  | Letter.aleph  => "ʔ"
  | Letter.bet    => "b"
  | Letter.gimel  => "ɡ"
  | Letter.dalet  => "d"
  | Letter.he     => "h"
  | Letter.vav    => "w"
  | Letter.zayin  => "z"
  | Letter.het    => "χ"
  | Letter.tet    => "tˤ"
  | Letter.yod    => "j"
  | Letter.kaf    => "k"
  | Letter.lamed  => "l"
  | Letter.mem    => "m"
  | Letter.nun    => "n"
  | Letter.samekh => "sˤ"
  | Letter.ayin   => "ʕ"
  | Letter.pe     => "p"
  | Letter.tsadi  => "tsˤ"
  | Letter.qof    => "q"
  | Letter.resh   => "r"
  | Letter.shin   => "ʃ"
  | Letter.tav    => "t"

/-- Letters are totally ordered -/
instance : LinearOrder Letter := by
  sorry  -- To be proven in Decidability module

end AramaicInvariant
