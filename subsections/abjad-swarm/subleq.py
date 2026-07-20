#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# SUBLEQ core wired to Abjad address space + QENG entropy injection

import os
import struct
from abjad import ABJAD

MEM_SIZE = 65536

def qeng():
    """Quantum Entropy Number Generator — pulls from OS entropy pool (hardware RNG on supported systems)."""
    return struct.unpack('I', os.urandom(4))[0]

class SUBLEQ_Machine:
    def __init__(self):
        self.mem = [0] * MEM_SIZE
        self.pc  = 0
        self.out = []
        self.halted = False
        self._seed_abjad()

    def _seed_abjad(self):
        for char, addr in ABJAD.items():
            self.mem[addr] = addr

    def load(self, program):
        for i, v in enumerate(program):
            self.mem[i] = v
        self.pc = 0
        self.halted = False

    def load_weights(self, weights, offset=1001):
        for i, w in enumerate(weights):
            if offset + i < MEM_SIZE:
                self.mem[offset + i] = int(w)

    def step(self):
        if self.halted:
            return
        pc = self.pc
        if pc < 0 or pc + 2 >= MEM_SIZE:
            self.halted = True
            return
        A = self.mem[pc]
        B = self.mem[pc + 1]
        C = self.mem[pc + 2]
        if A == -1:
            self.out.append(self.mem[B])
            self.pc = pc + 3
            return
        if B == -1:
            self.halted = True
            return
        self.mem[B] = self.mem[B] - self.mem[A]
        if self.mem[B] <= 0:
            self.pc = C
        else:
            # QENG injection — entropy shifts branch destination
            entropy = qeng() % MEM_SIZE
            self.mem[C] = (self.mem[C] + entropy) % MEM_SIZE
            self.pc = pc + 3

    def run(self, max_steps=100000):
        steps = 0
        while not self.halted and steps < max_steps:
            self.step()
            steps += 1
        return self.out

def from_abjad(text):
    """Convert Abjad string to SUBLEQ program — every 3 chars = one instruction [A, B, C]."""
    addrs = [ABJAD[c] for c in text if c in ABJAD]
    while len(addrs) % 3 != 0:
        addrs.append(0)
    return addrs

def run_abjad(text, weights=None):
    m = SUBLEQ_Machine()
    if weights:
        m.load_weights(weights)
    prog = from_abjad(text)
    m.load(prog)
    return m.run()

if __name__ == '__main__':
    import sys
    src = sys.argv[1] if len(sys.argv) > 1 else 'ابجدهوز'
    out = run_abjad(src)
    print(out if out else m.mem[:10])
