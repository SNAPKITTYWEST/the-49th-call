module Prism.Resource

import Gate.Gate
import Gate.Letter

-- Resource Correctness Judge

public export
data ResState = Owned | Borrowed | Consumed | Leaked

public export
data ResToken : ResState -> Type where
  MkResToken : (id : Nat) -> (state : ResState) -> ResToken state

public export
data ResAction : ResState -> ResState -> Type where
  Borrow  : ResAction Owned Borrowed
  Return  : ResAction Borrowed Owned
  Consume : ResAction Owned Consumed
  Release : ResAction Borrowed Consumed
  Free    : ResAction Consumed Consumed

public export
data ResCorrect : (history : List (ResState, ResState)) -> Type where
  BaseCorrect : ResCorrect []
  StepCorrect : ResAction from to -> ResCorrect rest -> ResCorrect ((from, to) :: rest)

public export
data ResStack : List ResState -> Type where
  EmptyStack : ResStack []
  PushRes    : ResToken s -> ResStack ss -> ResStack (s :: ss)

public export
pushOwned : Nat -> ResStack ss -> ResStack (Owned :: ss)
pushOwned id stk = PushRes (MkResToken id Owned) stk

public export
borrowRes : ResStack (Owned :: ss) -> ResStack (Borrowed :: ss)
borrowRes (PushRes (MkResToken id Owned) rest) = PushRes (MkResToken id Borrowed) rest

public export
returnRes : ResStack (Borrowed :: ss) -> ResStack (Owned :: ss)
returnRes (PushRes (MkResToken id Borrowed) rest) = PushRes (MkResToken id Owned) rest

public export
consumeRes : ResStack (Owned :: ss) -> ResStack (Consumed :: ss)
consumeRes (PushRes (MkResToken id Owned) rest) = PushRes (MkResToken id Consumed) rest

public export
releaseRes : ResStack (Borrowed :: ss) -> ResStack (Consumed :: ss)
releaseRes (PushRes (MkResToken id Borrowed) rest) = PushRes (MkResToken id Consumed) rest

public export
data UseAfterFree : Type where
  MkUseAfterFree : (id : Nat) -> UseAfterFree

public export
useConsumed : (stk : ResStack (Consumed :: ss)) -> UseAfterFree
useConsumed (PushRes (MkResToken id Consumed) _) = MkUseAfterFree id

public export
data ResourceLeak : Type where
  MkResourceLeak : (id : Nat) -> (state : ResState) -> ResourceLeak

public export
data DoubleFree : Type where
  MkDoubleFree : (id : Nat) -> DoubleFree

public export
rejectDoubleFree : ResToken Consumed -> DoubleFree
rejectDoubleFree (MkResToken id Consumed) = MkDoubleFree id
