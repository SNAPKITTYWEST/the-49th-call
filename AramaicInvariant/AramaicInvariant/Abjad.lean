-- Layer 2: Abjad Gematria (numerical valuation)
import AramaicInvariant.Alphabet
namespace AramaicInvariant
def Letter.abjad_value : Letter → ℕ
  | Letter.aleph  => 1
  | Letter.bet    => 2
  | Letter.gimel  => 3
  | Letter.dalet  => 4
  | Letter.he     => 5
  | Letter.vav    => 6
  | Letter.zayin  => 7
  | Letter.het    => 8
  | Letter.tet    => 9
  | Letter.yod    => 10
  | Letter.kaf    => 20
  | Letter.lamed  => 30
  | Letter.mem    => 40
  | Letter.nun    => 50
  | Letter.samekh => 60
  | Letter.ayin   => 70
  | Letter.pe     => 80
  | Letter.tsadi  => 90
  | Letter.qof    => 100
  | Letter.resh   => 200
  | Letter.shin   => 300
  | Letter.tav    => 400
def Root.abjad_sum : Root → ℕ
  | Root.tri r => r.c1.abjad_value + r.c2.abjad_value + r.c3.abjad_value
  | Root.quad r => r.c1.abjad_value + r.c2.abjad_value + r.c3.abjad_value + r.c4.abjad_value
end AramaicInvariant
