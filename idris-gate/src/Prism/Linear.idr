module Prism.Linear

import Gate.Gate
import Gate.Letter

-- Linear Proof Objects

public export
data Lin (a : Type) where
  MkLin : (val : a) -> Lin a

public export
linUnwrap : Lin a -> (a -> b) -> b
linUnwrap (MkLin val) f = f val

public export
linCompose : Lin a -> Lin b -> (a -> b -> c) -> c
linCompose (MkLin va) (MkLin vb) f = f va vb

public export
data LinDup : Type where
  MkLinDup : (id : Nat) -> LinDup

public export
rejectDup : Lin a -> LinDup
rejectDup (MkLin _) = MkLinDup 0

public export
data LinLoss : Type where
  MkLinLoss : (id : Nat) -> LinLoss

public export
data Proof : (prop : Type) -> Type where
  MkProof : prop -> Proof prop

public export
consumeProof : Proof prop -> (prop -> conclusion) -> conclusion
consumeProof (MkProof p) f = f p

public export
data ProofReuse : Type where
  MkProofReuse : (prop : Type) -> ProofReuse

public export
rejectReuse : Proof prop -> Proof prop -> ProofReuse
rejectReuse _ _ = MkProofReuse prop

public export
data GateProof : Gate -> Type where
  MkGateProof : (g : Gate) -> GateProof g

public export
consumeGateProof : GateProof g -> (Gate -> prop) -> prop
consumeGateProof (MkGateProof g) f = f g

public export
data AbjadProof : (a, b : Letter) -> (sum : Nat) -> Type where
  MkAbjadProof : (a : Letter) -> (b : Letter) -> (sum : Nat)
              -> {auto prf : abjad a + abjad b === sum}
              -> AbjadProof a b sum

public export
data OXOProof : Type where
  MkOXOProof : (l : Letter) -> {auto prf : l === Ayin} -> OXOProof

public export
theOXOProof : OXOProof
theOXOProof = MkOXOProof Ayin

public export
data MetatronProof : Gate -> Type where
  MkMetatron : (g : Gate)
            -> (enochianA : enochian_letter (gateA g) _)
            -> (enochianB : enochian_letter (gateB g) _)
            -> (arabicA : arabic_root (gateA g) _)
            -> (arabicB : arabic_root (gateB g) _)
            -> MetatronProof g

public export
data LinStack : List Type -> Type where
  EmptyLinStack : LinStack []
  PushLin       : Lin a -> LinStack as -> LinStack (a :: as)

public export
popLin : LinStack (a :: as) -> (Lin a, LinStack as)
popLin (PushLin v rest) = (v, rest)

public export
pushLin : Lin a -> LinStack as -> LinStack (a :: as)
pushLin = PushLin

public export
data LinExhausted : List Type -> Type where
  MkLinExhausted : LinExhausted []
