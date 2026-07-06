module Prism.Memory

import Gate.Gate
import Gate.Letter

-- Memory Correctness Judge

public export
data MemCell : Nat -> Type where
  MkCell : (addr : Nat) -> (size : Nat) -> MemCell size

public export
data MemState = Uninitialized | Allocated | Readable | Freed

public export
data MemProof : MemState -> Type where
  MkMemProof : (addr : Nat) -> (size : Nat) -> MemProof state

public export
data InBounds : Nat -> Nat -> Nat -> Type where
  MkInBounds : (addr : Nat) -> (offset : Nat) -> (size : Nat)
            -> {auto prf : (offset < size) === True}
            -> InBounds addr offset size

public export
data OutOfBounds : Type where
  MkOutOfBounds : (addr : Nat) -> (offset : Nat) -> (size : Nat) -> OutOfBounds

public export
alloc : (size : Nat) -> MemProof Uninitialized -> MemProof Allocated
alloc size (MkMemProof addr _) = MkMemProof addr size

public export
free : MemProof Readable -> MemProof Freed
free (MkMemProof addr size) = MkMemProof addr size

public export
data UseAfterFree : Type where
  MkUAF : (addr : Nat) -> UseAfterFree

public export
useFreed : MemProof Freed -> UseAfterFree
useFreed (MkMemProof addr _) = MkUAF addr

public export
data DoubleFree : Type where
  MkDF : (addr : Nat) -> DoubleFree

public export
doubleFree : MemProof Freed -> DoubleFree
doubleFree (MkMemProof addr _) = MkDF addr

public export
data Aligned : Nat -> Nat -> Type where
  MkAligned : (addr : Nat) -> (alignment : Nat)
           -> {auto prf : (addr `mod` alignment) === 0}
           -> Aligned addr alignment

public export
data UnalignedAccess : Type where
  MkUnaligned : (addr : Nat) -> (alignment : Nat) -> UnalignedAccess

public export
data StackFrame : Type where
  MkFrame : (depth : Nat) -> (vars : Nat) -> StackFrame

public export
pushFrame : StackFrame -> StackFrame
pushFrame (MkFrame depth vars) = MkFrame (S depth) 0

public export
popFrame : StackFrame -> StackFrame
popFrame (MkFrame 0 _) = MkFrame 0 0
popFrame (MkFrame (S d) _) = MkFrame d 0

public export
data NullPtr : Type where
  MkNullPtr : NullPtr

public export
dereferenceNull : NullPtr
dereferenceNull = MkNullPtr
