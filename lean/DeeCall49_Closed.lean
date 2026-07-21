import Mathlib.Analysis.SpecialFunctions.Sqrt
import Mathlib.Tactic

namespace DeeCall49

theorem call49_product (alpha beta : Real) (ha : 0 < alpha) (hb : 0 < beta) (h_gt : beta < alpha) :
    (Real.sqrt alpha + Real.sqrt beta) * (Real.sqrt alpha - Real.sqrt beta) = alpha - beta := by
  have h1 := Real.sq_sqrt (le_of_lt ha)
  have h2 := Real.sq_sqrt (le_of_lt hb)
  nlinarith [Real.sqrt_nonneg alpha, Real.sqrt_nonneg beta, Real.sqrt_pos.mpr ha, Real.sqrt_pos.mpr hb]

theorem has_rational_square_sqrt (q : Rat) (hq : 0 < q) :
    exists (r : Rat), (Real.sqrt (q : Real))^2 = (r : Real) := by
  exact ⟨q, Real.sq_sqrt (by exact_mod_cast le_of_lt hq)⟩

theorem binomial_terms_exist (alpha beta : Rat) (ha : 0 < alpha) (hb : 0 < beta)
    (h_incomm : forall (q : Rat), (alpha : Real) ≠ q^2 * beta) :
    exists (rho sigma : Real),
      rho > 0 ∧ sigma > 0 ∧
      rho + sigma = Real.sqrt alpha + Real.sqrt beta ∧
      (exists q : Rat, rho^2 = (q : Real)) ∧
      (exists q : Rat, sigma^2 = (q : Real)) := by
  refine ⟨Real.sqrt alpha, Real.sqrt beta, Real.sqrt_pos.mpr (by exact_mod_cast ha),
          Real.sqrt_pos.mpr (by exact_mod_cast hb), rfl, ?_, ?_⟩
  · exact ⟨alpha, Real.sq_sqrt (by exact_mod_cast le_of_lt ha)⟩
  · exact ⟨beta, Real.sq_sqrt (by exact_mod_cast le_of_lt hb)⟩

end DeeCall49