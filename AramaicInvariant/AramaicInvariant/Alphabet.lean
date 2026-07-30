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

/-- Weak radicals (semivowels: א, ה, ו, י) -/
def Letter.isWeak : Letter → Bool
  | Letter.aleph | Letter.he | Letter.vav | Letter.yod => true
  | _ => false

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

/-- Letters are totally ordered (decidable lexicographic by enum index) -/
def letterOrd : Letter → Letter → Ordering
  | Letter.aleph, Letter.aleph => Ordering.eq
  | Letter.aleph, _ => Ordering.lt
  | _, Letter.aleph => Ordering.gt
  | Letter.bet, Letter.bet => Ordering.eq
  | Letter.bet, _ => Ordering.lt
  | _, Letter.bet => Ordering.gt
  | Letter.gimel, Letter.gimel => Ordering.eq
  | Letter.gimel, _ => Ordering.lt
  | _, Letter.gimel => Ordering.gt
  | Letter.dalet, Letter.dalet => Ordering.eq
  | Letter.dalet, _ => Ordering.lt
  | _, Letter.dalet => Ordering.gt
  | Letter.he, Letter.he => Ordering.eq
  | Letter.he, _ => Ordering.lt
  | _, Letter.he => Ordering.gt
  | Letter.vav, Letter.vav => Ordering.eq
  | Letter.vav, _ => Ordering.lt
  | _, Letter.vav => Ordering.gt
  | Letter.zayin, Letter.zayin => Ordering.eq
  | Letter.zayin, _ => Ordering.lt
  | _, Letter.zayin => Ordering.gt
  | Letter.het, Letter.het => Ordering.eq
  | Letter.het, _ => Ordering.lt
  | _, Letter.het => Ordering.gt
  | Letter.tet, Letter.tet => Ordering.eq
  | Letter.tet, _ => Ordering.lt
  | _, Letter.tet => Ordering.gt
  | Letter.yod, Letter.yod => Ordering.eq
  | Letter.yod, _ => Ordering.lt
  | _, Letter.yod => Ordering.gt
  | Letter.kaf, Letter.kaf => Ordering.eq
  | Letter.kaf, _ => Ordering.lt
  | _, Letter.kaf => Ordering.gt
  | Letter.lamed, Letter.lamed => Ordering.eq
  | Letter.lamed, _ => Ordering.lt
  | _, Letter.lamed => Ordering.gt
  | Letter.mem, Letter.mem => Ordering.eq
  | Letter.mem, _ => Ordering.lt
  | _, Letter.mem => Ordering.gt
  | Letter.nun, Letter.nun => Ordering.eq
  | Letter.nun, _ => Ordering.lt
  | _, Letter.nun => Ordering.gt
  | Letter.samekh, Letter.samekh => Ordering.eq
  | Letter.samekh, _ => Ordering.lt
  | _, Letter.samekh => Ordering.gt
  | Letter.ayin, Letter.ayin => Ordering.eq
  | Letter.ayin, _ => Ordering.lt
  | _, Letter.ayin => Ordering.gt
  | Letter.pe, Letter.pe => Ordering.eq
  | Letter.pe, _ => Ordering.lt
  | _, Letter.pe => Ordering.gt
  | Letter.tsadi, Letter.tsadi => Ordering.eq
  | Letter.tsadi, _ => Ordering.lt
  | _, Letter.tsadi => Ordering.gt
  | Letter.qof, Letter.qof => Ordering.eq
  | Letter.qof, _ => Ordering.lt
  | _, Letter.qof => Ordering.gt
  | Letter.resh, Letter.resh => Ordering.eq
  | Letter.resh, _ => Ordering.lt
  | _, Letter.resh => Ordering.gt
  | Letter.shin, Letter.shin => Ordering.eq
  | Letter.shin, _ => Ordering.lt
  | _, Letter.shin => Ordering.gt
  | Letter.tav, Letter.tav => Ordering.eq

instance : LinearOrder Letter :=
  { le := fun a b => letterOrd a b ≠ Ordering.gt
    le_refl := fun a => by simp [letterOrd]; decide
    le_trans := fun a b c _ _ => by simp [letterOrd]; decide
    le_antisymm := fun a b _ _ => by simp [letterOrd]; decide
    decidableLE := fun a b => by decide
    compare := letterOrd
    compare_eq_lt_iff_lt := fun a b => by simp [letterOrd]; decide
  }

end AramaicInvariant
