#!/usr/bin/env python3
# -*- coding: utf-8 -*-

ABJAD = {
    'ا':1,  'ب':2,  'ج':3,  'د':4,  'ه':5,  'و':6,  'ز':7,  'ح':8,  'ط':9,
    'ي':10, 'ك':20, 'ل':30, 'م':40, 'ن':50, 'س':60, 'ع':70, 'ف':80, 'ص':90,
    'ق':100,'ر':200,'ش':300,'ت':400,'ث':500,'خ':600,'ذ':700,'ض':800,'ظ':900,'غ':1000
}

MACROS = {
    1:   lambda s, k: s.push(k),
    2:   lambda s, k: s.push(s.pop() + s.pop()),
    3:   lambda s, k: s.push(s.pop() * s.pop()),
    4:   lambda s, k: s.push(s.peek()),
    5:   lambda s, k: s.pop(),
    6:   lambda s, k: s.swap(),
    7:   lambda s, k: s.emit(),
    8:   lambda s, k: s.load(k),
    9:   lambda s, k: s.exec_str(),
    10:  lambda s, k: s.push(s.pop() - s.pop()),
    20:  lambda s, k: s.push(0 if s.pop() == 0 else 1),
    30:  lambda s, k: s.push(s.pop() % k if k else 0),
    40:  lambda s, k: s.rot(),
    50:  lambda s, k: s.push(s.depth()),
    60:  lambda s, k: s.push(s.pop() ** 2),
    70:  lambda s, k: s.push(abs(s.pop())),
    80:  lambda s, k: s.push(s.pop() << 1),
    90:  lambda s, k: s.push(s.pop() >> 1),
    100: lambda s, k: s.push(sum(ABJAD.get(c, 0) for c in s.str_reg)),
    200: lambda s, k: s.reverse_str(),
    300: lambda s, k: s.push(len(s.str_reg)),
    400: lambda s, k: s.str_reg.__setitem__(slice(None), list(s.str_reg + chr(s.pop()))),
    500: lambda s, k: s.noop(),
    600: lambda s, k: s.noop(),
    700: lambda s, k: s.noop(),
    800: lambda s, k: s.noop(),
    900: lambda s, k: s.noop(),
    1000:lambda s, k: s.noop(),
}

class Machine:
    def __init__(self):
        self.stack   = []
        self.str_reg = []
        self.out     = []

    def push(self, v):   self.stack.append(v)
    def pop(self):       return self.stack.pop() if self.stack else 0
    def peek(self):      return self.stack[-1] if self.stack else 0
    def depth(self):     return len(self.stack)
    def swap(self):
        if len(self.stack) >= 2:
            self.stack[-1], self.stack[-2] = self.stack[-2], self.stack[-1]
    def rot(self):
        if len(self.stack) >= 3:
            self.stack[-3], self.stack[-2], self.stack[-1] = \
            self.stack[-2], self.stack[-1], self.stack[-3]
    def emit(self):      self.out.append(self.pop())
    def load(self, k):   self.str_reg = [k]
    def exec_str(self):  run(self, self.str_reg)
    def reverse_str(self): self.str_reg = self.str_reg[::-1]
    def noop(self):      pass

def run(machine, opcodes):
    for k in opcodes:
        fn = MACROS.get(k)
        if fn:
            fn(machine, k)

def execute(source):
    m = Machine()
    ops = [ABJAD[c] for c in source if c in ABJAD]
    run(m, ops)
    return m.stack, m.out

if __name__ == '__main__':
    import sys
    src = sys.argv[1] if len(sys.argv) > 1 else 'ابجد'
    stack, out = execute(src)
    for v in out:
        print(v, end=' ')
    if out:
        print()
    print(stack)
