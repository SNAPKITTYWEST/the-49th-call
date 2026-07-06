module Prism.CExec

import Gate.Gate
import Gate.Letter

-- C-Level Execution Discipline

public export
data FFIResult : Type -> Type where
  FFIOk     : a -> FFIResult a
  FFIFail   : String -> FFIResult a
  FFISignal : Int -> FFIResult a

public export
data FFICall : (args : Type) -> (ret : Type) -> Type where
  MkFFICall : (name : String) -> (fn : args -> IO ret) -> FFICall args ret

public export
data FFISafe : FFICall args ret -> Type where
  MkFFISafe : (call : FFICall args ret) -> (argsSafe : args) -> FFISafe call

public export
data MemContract : Type where
  NoAlloc    : MemContract
  MayAlloc   : (maxSize : Nat) -> MemContract
  MayFree    : MemContract
  MayRealloc : MemContract

public export
data SyscallContract : Type where
  MkSyscall : (name : String) -> (inputSize : Nat) -> (outputSize : Nat)
           -> (canFail : Bool) -> SyscallContract

public export
SYS_READ : SyscallContract
SYS_READ = MkSyscall "read" 4 4 True

public export
SYS_WRITE : SyscallContract
SYS_WRITE = MkSyscall "write" 4 4 True

public export
SYS_OPEN : SyscallContract
SYS_OPEN = MkSyscall "open" 8 4 True

public export
SYS_CLOSE : SyscallContract
SYS_CLOSE = MkSyscall "close" 4 4 True

public export
SYS_MMAP : SyscallContract
SYS_MMAP = MkSyscall "mmap" 24 8 True

public export
SYS_MUNMAP : SyscallContract
SYS_MUNMAP = MkSyscall "munmap" 8 4 True

public export
SYS_BRK : SyscallContract
SYS_BRK = MkSyscall "brk" 4 4 True

public export
data StackDepth : Nat -> Type where
  MkStackDepth : (depth : Nat) -> {auto max : (depth <= 1024) === True} -> StackDepth depth

public export
data StackOverflow : Type where
  MkStackOverflow : (depth : Nat) -> StackOverflow

public export
checkStackDepth : (depth : Nat) -> Either StackOverflow (StackDepth depth)
checkStackDepth d =
  case the (Dec (d <= 1024)) (decLTE d 1024) of
    Yes prf => Right (MkStackDepth d)
    No  _   => Left (MkStackOverflow d)

public export
data BufferBounds : Type where
  MkBuffer : (base : Nat) -> (size : Nat) -> BufferBounds

public export
data InBuffer : BufferBounds -> Nat -> Nat -> Type where
  MkInBuffer : (buf : BufferBounds) -> (offset : Nat) -> (accessSize : Nat)
            -> {auto prf1 : (offset < buf.size) === True}
            -> {auto prf2 : (offset + accessSize <= buf.size) === True}
            -> InBuffer buf offset accessSize

public export
data BufferOverflow : Type where
  MkBufferOverflow : (base : Nat) -> (offset : Nat) -> (size : Nat) -> BufferOverflow

public export
data Signal = SIGTERM | SIGSEGV | SIGFPE | SIGINT | SIGABRT | SIGUSR1 | SIGUSR2

public export
data SignalSafe : Signal -> Type where
  MkSignalSafe : (sig : Signal) -> SignalSafe sig

public export
data ExecStep : Type where
  StepFFI     : String -> ExecStep
  StepSyscall : String -> ExecStep
  StepAlloc   : Nat -> ExecStep
  StepFree    : Nat -> ExecStep
  StepBranch  : Bool -> ExecStep
  StepLoop    : Nat -> ExecStep

public export
data ExecTrace : List ExecStep -> Type where
  EmptyTrace : ExecTrace []
  StepTrace : ExecStep -> ExecTrace rest -> ExecTrace (step :: rest)

public export
data ExecDisciplined : List ExecStep -> Type where
  BaseDisciplined : ExecDisciplined []
  StepDisciplined : ExecStep -> ExecDisciplined rest -> ExecDisciplined (step :: rest)

public export
data NullDeref : Type where
  MkNullDeref : (addr : Nat) -> NullDeref

public export
data IntOverflow : Type where
  MkIntOverflow : (op : String) -> (a, b : Nat) -> IntOverflow

public export
data DivByZero : Type where
  MkDivByZero : (op : String) -> DivByZero

public export
data StackSmash : Type where
  MkStackSmash : (expected : Nat) -> (actual : Nat) -> StackSmash

public export
data BadReturn : Type where
  MkBadReturn : (expected : Nat) -> (actual : Nat) -> BadReturn
