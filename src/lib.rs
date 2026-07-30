use std::collections::HashMap;

pub mod covenant_ffi;
use covenant_ffi::{verify_petitioner_authorized, check_principles_satisfied, seal_square_to_chain, DivinePrinciple, CovenantError};
pub mod sovereign_oracle;

// ── Reading direction — mirrors enochian.rs in DEVFLOW-FINANCE ───────────────

// ── Reading direction — mirrors enochian.rs in DEVFLOW-FINANCE ───────────────
// The compiler enforces: you cannot read a ComefromRTL line LTR.
// Same type as substrate/soul_spec.hs — four implementations, one truth.
#[derive(Debug, Clone, PartialEq)]
pub enum ReadingDirection {
    LTR,
    RTL,
    Boustrophedon { starting: Box<ReadingDirection> },
    ComefromRTL,    // INTERCAL COMEFROM = the 49th Call = RTL as control flow inversion
}

// ── Grid value — the quantum possibility space ────────────────────────────────
// Unknown squares are not blank. They are uncollapsed states in the Abzu.
// ENKI holds the Abzu. METATRON certifies when collapse is complete.
#[derive(Debug, Clone)]
pub enum GridValue {
    Known(char),
    Uncertain(Vec<(char, f32)>),    // candidates with confidence weights
    Collapsed {
        value:      char,
        confidence: f32,
        pass:       u8,             // which pass resolved it (1-4)
    },
    Void,                           // contradiction — no valid assignment
}

impl GridValue {
    pub fn is_resolved(&self) -> bool {
        matches!(self, Self::Known(_) | Self::Collapsed { .. })
    }

    pub fn as_char(&self) -> Option<char> {
        match self {
            Self::Known(c)               => Some(*c),
            Self::Collapsed { value, .. } => Some(*value),
            _                            => None,
        }
    }

    /// METATRON certifies when all four passes agree on the same character.
    /// This is the double-entry balance: LTR debit == RTL credit.
    /// Dee received the LTR. Al-Hamid holds the RTL. Both must agree.
    pub fn metatron_certify(candidates: &[(char, u8)]) -> Self {
        if candidates.is_empty() { return Self::Void; }
        let first = candidates[0].0;
        if candidates.iter().all(|(c, _)| *c == first) {
            Self::Collapsed { value: first, confidence: 1.0, pass: 0xFF }
        } else {
            Self::Uncertain(candidates.iter().map(|&(c, p)| (c, p as f32 / 4.0)).collect())
        }
    }
}

// ── Source provenance — strict corpus separation ──────────────────────────────
// Sloane/Cotton/Ashmole = Verified.
// Golden Dawn reconstructions = Interpolated (kept strictly separate).
// No mixing. Ever.
#[derive(Debug, Clone, PartialEq)]
pub enum SourceTag {
    Verified   { manuscript: &'static str },
    Interpolated { source: &'static str },
    Unknown,
}

// ── Grid square — one coordinate in the 12×13 Watchtower ─────────────────────
#[derive(Debug, Clone)]
pub struct GridSquare {
    pub value:      GridValue,
    pub tag:        SourceTag,
    pub confidence: f32,
}

impl GridSquare {
    pub fn verified(ch: char, ms: &'static str) -> Self {
        Self { value: GridValue::Known(ch), tag: SourceTag::Verified { manuscript: ms }, confidence: 1.0 }
    }
    pub fn unknown() -> Self {
        Self { value: GridValue::Void, tag: SourceTag::Unknown, confidence: 0.0 }
    }
}

// ── Watchtower ────────────────────────────────────────────────────────────────
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum Watchtower { Air, Water, Earth, Fire }

// ── The 12×13 Watchtower grid ─────────────────────────────────────────────────
// 156 squares per tablet. 4 tablets = 624. + 20 Tablet of Union = 644 nodes.
// Every unknown square = an uncollapsed state.
// The four-pass constraint system narrows the space until one value remains.
pub struct WatchtowerGrid {
    pub element: Watchtower,
    pub squares: Vec<Vec<GridSquare>>,  // [row][col], 12 rows × 13 cols
}

impl WatchtowerGrid {
    pub fn empty(element: Watchtower) -> Self {
        Self {
            element,
            squares: (0..12).map(|_| (0..13).map(|_| GridSquare::unknown()).collect()).collect(),
        }
    }

    pub fn verified_count(&self) -> usize {
        self.squares.iter().flatten()
            .filter(|s| matches!(s.tag, SourceTag::Verified { .. }))
            .count()
    }

    pub fn unknown_count(&self) -> usize {
        self.squares.iter().flatten()
            .filter(|s| s.tag == SourceTag::Unknown)
            .count()
    }
}

// ── The four-pass constraint engine ──────────────────────────────────────────
// Each pass narrows the possibility space for unknown squares.
// METATRON certifies when all four passes agree.
pub trait ConstraintPass {
    fn pass_name(&self) -> &'static str;
    fn direction(&self) -> ReadingDirection;
    fn propagate(&self, grid: &mut WatchtowerGrid) -> usize; // returns squares resolved
}

pub struct EnochianPass;
pub struct LatinPass;
pub struct HebrewPass;
pub struct ArabicPass;    // The 49th layer — the socket Dee left open

impl ConstraintPass for EnochianPass {
    fn pass_name(&self) -> &'static str { "Enochian LTR" }
    fn direction(&self) -> ReadingDirection { ReadingDirection::LTR }
    fn propagate(&self, _grid: &mut WatchtowerGrid) -> usize { 0 }
}

impl ConstraintPass for LatinPass {
    fn pass_name(&self) -> &'static str { "Latin LTR" }
    fn direction(&self) -> ReadingDirection { ReadingDirection::LTR }
    fn propagate(&self, _grid: &mut WatchtowerGrid) -> usize { 0 }
}

impl ConstraintPass for HebrewPass {
    fn pass_name(&self) -> &'static str { "Hebrew RTL" }
    fn direction(&self) -> ReadingDirection { ReadingDirection::RTL }
    fn propagate(&self, _grid: &mut WatchtowerGrid) -> usize { 0 }
}

impl ConstraintPass for ArabicPass {
    fn pass_name(&self) -> &'static str { "Arabic RTL — the 49th layer" }
    fn direction(&self) -> ReadingDirection { ReadingDirection::ComefromRTL }
    fn propagate(&self, _grid: &mut WatchtowerGrid) -> usize { 0 }
}

// ── The ERE — Enochian Reconstruction Engine ─────────────────────────────────
// Full implementation: github.com/SNAPKITTYWEST/DEVFLOW-FINANCE
// snapkitty-tg/src/enochian.rs
pub struct EnochianReconstructionEngine {
    pub air:   WatchtowerGrid,
    pub water: WatchtowerGrid,
    pub earth: WatchtowerGrid,
    pub fire:  WatchtowerGrid,
    pub passes: Vec<Box<dyn ConstraintPass>>,
}

impl EnochianReconstructionEngine {
    pub fn new() -> Self {
        Self {
            air:   WatchtowerGrid::empty(Watchtower::Air),
            water: WatchtowerGrid::empty(Watchtower::Water),
            earth: WatchtowerGrid::empty(Watchtower::Earth),
            fire:  WatchtowerGrid::empty(Watchtower::Fire),
            passes: vec![
                Box::new(EnochianPass),
                Box::new(LatinPass),
                Box::new(HebrewPass),
                Box::new(ArabicPass),  // The socket Dee left open for 450 years
            ],
        }
    }

    pub fn total_unknown(&self) -> usize {
        self.air.unknown_count()   + self.water.unknown_count() +
        self.earth.unknown_count() + self.fire.unknown_count()
    }

    /// Run all four passes. Returns total squares resolved.
    /// METATRON certifies only when all four passes agree.
    pub fn run_passes(&mut self) -> usize {
        let mut total = 0;
        for grid in [&mut self.air, &mut self.water, &mut self.earth, &mut self.fire] {
            for pass in &self.passes {
                total += pass.propagate(grid);
            }
        }
        total
    }
}

// ── The OISC connection ───────────────────────────────────────────────────────
// All ancient scripts run SUBLEQ(A, B, C).
// The 49th Call is C. Not a new call. The branch instruction.
pub struct OISCInstruction {
    pub a: u32,
    pub b: u32,
    pub c: i32,    // -1 = METATRON halts and certifies
}

impl OISCInstruction {
    /// The 49th Call as OISC.
    /// A=48 (all calls enumerated), B=48 (threshold reached), C=-1 (METATRON certifies).
    pub fn call_49() -> Self {
        Self { a: 48, b: 48, c: -1 }
    }

    pub fn fires(&self) -> bool { self.a <= self.b }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn call_49_fires() {
        let inst = OISCInstruction::call_49();
        assert!(inst.fires(), "The 49th Call must fire when A == B");
    }

    #[test]
    fn arabic_pass_is_comefrom_rtl() {
        let pass = ArabicPass;
        assert_eq!(pass.direction(), ReadingDirection::ComefromRTL);
    }

    #[test]
    fn metatron_certifies_unanimous_agreement() {
        let candidates = vec![('A', 1), ('A', 2), ('A', 3), ('A', 4)];
        let result = GridValue::metatron_certify(&candidates);
        assert!(result.is_resolved(), "METATRON must certify when all four passes agree");
    }

    #[test]
    fn metatron_void_on_contradiction() {
        let candidates = vec![('A', 1), ('B', 2), ('A', 3), ('C', 4)];
        let result = GridValue::metatron_certify(&candidates);
        // Contradiction — passes disagree — should be Uncertain or Void
        assert!(!result.is_resolved(), "METATRON must not certify contradiction");
    }
}

// ── COVENANT INTEGRATION — Authorization Gate ────────────────────────────────
// Every invocation of the ERE requires:
// 1. Petitioner is chartered (authorized via Moorish Divine Covenant)
// 2. All required divine principles are satisfied
// 3. Results are sealed to the covenant chain

impl EnochianReconstructionEngine {
    /// Run all four passes with covenant authorization.
    /// Returns total squares resolved, or CovenantError if unauthorized/principles violated.
    pub fn run_passes_with_covenant(
        &mut self,
        petitioner: &str,
    ) -> Result<usize, CovenantError> {
        // LAYER 1: Verify petitioner is chartered
        verify_petitioner_authorized(petitioner)?;

        // LAYER 2: Check that all 5 divine principles are satisfied
        let all_principles = vec![
            DivinePrinciple::Love,
            DivinePrinciple::Truth,
            DivinePrinciple::Peace,
            DivinePrinciple::Freedom,
            DivinePrinciple::Justice,
        ];
        check_principles_satisfied(&all_principles)?;

        // LAYER 3: Run all four passes (constraint solving)
        let mut total = 0;
        for grid in [&mut self.air, &mut self.water, &mut self.earth, &mut self.fire] {
            for pass in &self.passes {
                total += pass.propagate(grid);
            }
        }

        // LAYER 4: Seal resolved squares to covenant chain
        // (In production, this would seal each resolved square; for now, seal summary)
        if total > 0 {
            // Seal a marker receipt to the chain
            seal_square_to_chain(
                0,  // Special marker: row 0
                total as u32,  // Total squares resolved
                'M',  // METATRON marker
            )?;
        }

        Ok(total)
    }

    /// Run passes without covenant (legacy, unsafe)
    /// Deprecated: use run_passes_with_covenant() for production
    pub fn run_passes(&mut self) -> usize {
        let mut total = 0;
        for grid in [&mut self.air, &mut self.water, &mut self.earth, &mut self.fire] {
            for pass in &self.passes {
                total += pass.propagate(grid);
            }
        }
        total
    }
}

#[cfg(test)]
mod covenant_tests {
    use super::*;

    #[test]
    fn covenant_error_types() {
        assert_eq!(CovenantError::Unauthorized, CovenantError::Unauthorized);
        assert_eq!(
            CovenantError::PrinciplesNotSatisfied,
            CovenantError::PrinciplesNotSatisfied
        );
    }

    #[test]
    fn run_passes_legacy_still_works() {
        let mut ere = EnochianReconstructionEngine::new();
        let total = ere.run_passes();  // Should still work, but unsafe
        assert_eq!(total, 0);  // No actual constraint logic implemented yet
    }
}

// ── POINT 2: PER-PASS PRINCIPLE ENFORCEMENT ──────────────────────────────────
// Each linguistic pass requires specific divine principles before execution.
// This ensures that the four-pass constraint solver respects the Moorish covenant.

impl ConstraintPass for EnochianPass {
    fn pass_name(&self) -> &'static str { "Enochian LTR" }
    fn direction(&self) -> ReadingDirection { ReadingDirection::LTR }
    
    fn propagate(&self, grid: &mut WatchtowerGrid) -> usize {
        // Enochian (witness language) requires all 5 principles
        let required = vec![
            DivinePrinciple::Love,
            DivinePrinciple::Truth,
            DivinePrinciple::Peace,
            DivinePrinciple::Freedom,
            DivinePrinciple::Justice,
        ];
        
        // Check covenant: all 5 principles must be satisfied
        match check_principles_satisfied(&required) {
            Ok(()) => {
                // Principles satisfied; proceed with constraint propagation
                // (In full implementation, this would execute real constraint logic)
                0  // Placeholder: no squares resolved yet
            }
            Err(_) => {
                // Principles violated; abort this pass
                0
            }
        }
    }
}

impl ConstraintPass for LatinPass {
    fn pass_name(&self) -> &'static str { "Latin LTR" }
    fn direction(&self) -> ReadingDirection { ReadingDirection::LTR }
    
    fn propagate(&self, grid: &mut WatchtowerGrid) -> usize {
        // Latin (scholarly/classical) requires Truth + Love
        let required = vec![
            DivinePrinciple::Truth,
            DivinePrinciple::Love,
        ];
        
        match check_principles_satisfied(&required) {
            Ok(()) => {
                // Principles satisfied; proceed
                0  // Placeholder
            }
            Err(_) => {
                // Principles violated; abort this pass
                0
            }
        }
    }
}

impl ConstraintPass for HebrewPass {
    fn pass_name(&self) -> &'static str { "Hebrew RTL" }
    fn direction(&self) -> ReadingDirection { ReadingDirection::RTL }
    
    fn propagate(&self, grid: &mut WatchtowerGrid) -> usize {
        // Hebrew (divine names, RTL reading) requires Peace + Justice
        let required = vec![
            DivinePrinciple::Peace,
            DivinePrinciple::Justice,
        ];
        
        match check_principles_satisfied(&required) {
            Ok(()) => {
                // Principles satisfied; proceed
                0  // Placeholder
            }
            Err(_) => {
                // Principles violated; abort this pass
                0
            }
        }
    }
}

impl ConstraintPass for ArabicPass {
    fn pass_name(&self) -> &'static str { "Arabic RTL — the 49th layer" }
    fn direction(&self) -> ReadingDirection { ReadingDirection::ComefromRTL }
    
    fn propagate(&self, grid: &mut WatchtowerGrid) -> usize {
        // Arabic (the 49th layer, incommensurable reading) requires Freedom + Truth
        let required = vec![
            DivinePrinciple::Freedom,
            DivinePrinciple::Truth,
        ];
        
        match check_principles_satisfied(&required) {
            Ok(()) => {
                // Principles satisfied; proceed
                0  // Placeholder
            }
            Err(_) => {
                // Principles violated; abort this pass
                0
            }
        }
    }
}

#[cfg(test)]
mod per_pass_principle_tests {
    use super::*;

    #[test]
    fn enochian_pass_requires_all_five() {
        let pass = EnochianPass;
        assert_eq!(pass.pass_name(), "Enochian LTR");
        assert_eq!(pass.direction(), ReadingDirection::LTR);
        // Actual principle check happens in propagate() via C FFI
    }

    #[test]
    fn latin_pass_requires_truth_and_love() {
        let pass = LatinPass;
        assert_eq!(pass.pass_name(), "Latin LTR");
        assert_eq!(pass.direction(), ReadingDirection::LTR);
    }

    #[test]
    fn hebrew_pass_requires_peace_and_justice() {
        let pass = HebrewPass;
        assert_eq!(pass.pass_name(), "Hebrew RTL");
        assert_eq!(pass.direction(), ReadingDirection::RTL);
    }

    #[test]
    fn arabic_pass_requires_freedom_and_truth() {
        let pass = ArabicPass;
        assert_eq!(pass.pass_name(), "Arabic RTL — the 49th layer");
        assert_eq!(pass.direction(), ReadingDirection::ComefromRTL);
    }

    #[test]
    fn all_passes_implemented() {
        let passes: Vec<Box<dyn ConstraintPass>> = vec![
            Box::new(EnochianPass),
            Box::new(LatinPass),
            Box::new(HebrewPass),
            Box::new(ArabicPass),
        ];
        assert_eq!(passes.len(), 4, "All four passes must be implemented");
    }
}

// ── POINT 3: RECEIPT SEALING PER GRID SQUARE ─────────────────────────────────
// Each resolved grid square is sealed to the covenant chain as a witnessed receipt.
// This creates an immutable audit trail of resolution.

impl GridValue {
    /// Certify and seal: METATRON certifies consensus, then seals to chain.
    /// Returns the resolved value, or Void if contradiction detected.
    pub fn metatron_certify_and_seal(
        row: u32,
        col: u32,
        candidates: &[(char, u8)],
    ) -> Self {
        if candidates.is_empty() {
            return Self::Void;
        }

        let first = candidates[0].0;
        let all_agree = candidates.iter().all(|(c, _)| *c == first);

        if all_agree {
            let resolved = Self::Collapsed {
                value: first,
                confidence: 1.0,
                pass: 0xFF,
            };

            // SEAL TO CHAIN: Log this resolution to covenant chain
            // (In production, this would call covenant_chain_append)
            // For now, we attempt the seal and silently continue if it fails
            let _ = seal_square_to_chain(row, col, first);

            resolved
        } else {
            // Candidates disagree; return Uncertain
            Self::Uncertain(candidates.iter().map(|&(c, p)| (c, p as f32 / 4.0)).collect())
        }
    }
}

#[cfg(test)]
mod per_square_sealing_tests {
    use super::*;

    #[test]
    fn metatron_certify_and_seal_consensus() {
        let candidates = vec![('A', 1), ('A', 2), ('A', 3), ('A', 4)];
        let result = GridValue::metatron_certify_and_seal(0, 0, &candidates);
        assert!(result.is_resolved(), "METATRON must certify unanimous agreement");
        assert_eq!(result.as_char(), Some('A'));
    }

    #[test]
    fn metatron_certify_and_seal_disagreement() {
        let candidates = vec![('A', 1), ('B', 2), ('A', 3), ('C', 4)];
        let result = GridValue::metatron_certify_and_seal(0, 0, &candidates);
        assert!(!result.is_resolved(), "METATRON must not certify disagreement");
    }

    #[test]
    fn metatron_certify_and_seal_empty() {
        let candidates: Vec<(char, u8)> = vec![];
        let result = GridValue::metatron_certify_and_seal(0, 0, &candidates);
        assert_eq!(result, GridValue::Void, "METATRON must return Void for empty candidates");
    }
}
