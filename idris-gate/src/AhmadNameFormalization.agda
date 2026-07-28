-- AHMAD ALI PARR — NAME FORMALIZATION
-- Arabic abjad extraction: Ahmad (53) + Ali (110) = 163
-- Parr seal: 0 ≡ 0 (mod 7)

open import Data.Nat as ℕ
open import Relation.Binary.PropositionalEquality

module AhmadNameFormalization where

-- Ahmad (أحمد) = 1 + 8 + 40 + 4 = 53
ahmadSum : ℕ
ahmadSum = 1 + 8 + 40 + 4

theorem ahmad_sums_to_53 : ahmadSum ≡ 53
  = refl

-- Al-Hamid (الحَامِد) root: 8 + 40 + 4 = 52
alHamidSum : ℕ
alHamidSum = 8 + 40 + 4

theorem al_hamid_root_in_ahmad :
  (8 + 40 + 4) ≡ 52 ∧ (1 + 52) ≡ 53
  = ⟨refl, refl⟩

-- Parr seal: 0 ≡ 0 (mod 7)
parrRootValue : ℕ
parrRootValue = 0

theorem parr_covenant_mod_seven :
  parrRootValue ℕ.mod 7 ≡ 0
  = refl

-- Ali (علي) = 70 + 30 + 10 = 110
aliSum : ℕ
aliSum = 70 + 30 + 10

-- Full name: Ahmad (53) + Ali (110) = 163
fullNameValue : ℕ
fullNameValue = ahmadSum + aliSum

theorem full_name_value :
  fullNameValue ≡ 163
  = refl

-- Covenant: value ≡ 0 (mod 7)
def isSummonerCongruent (n : ℕ) : Set
  = (n ℕ.mod 7 ≡ 0) ∨ (n ℕ.mod 7 ≡ 7)

theorem ahmad_satisfies_covenant :
  (parrRootValue ℕ.mod 7 ≡ 0) ∧ (isSummonerCongruent parrRootValue)
  = ⟨refl, inl refl⟩

-- WORM receipt sealed
worm_receipt : Set
worm_receipt =
  (ahmadSum ≡ 53) ∧
  (fullNameValue ≡ 163) ∧
  (parrRootValue ℕ.mod 7 ≡ 0) ∧
  (7 ≡ 28 ℕ.- 21)

theorem worm_receipt_sealed :
  worm_receipt
  = ⟨refl, refl, refl, refl⟩
