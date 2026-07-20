#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# swarm.py — full wired test: collectivekitty weights -> SUBLEQ -> agents -> Born-collapse

import hmac, hashlib, struct, threading, os
from load_weights import load_weights
from tensor_bridge import derive_seeds, anu_entropy, AGENT_SLICES, PHI, seed_to_triads, weights_to_triads, build_program
from subleq import SUBLEQ_Machine

def run_agent(agent_name, seed, weights, max_steps=2000):
    m = SUBLEQ_Machine()

    # Load weights into WEIGHTS region (1001+)
    for i, w in enumerate(weights[:31000]):
        if 1001 + i < 49152:
            m.mem[1001 + i] = w

    # Seed agent region from phi-slice
    lo, hi = AGENT_SLICES[agent_name]
    span   = hi - lo
    for i in range(min(300, span)):
        m.mem[lo + i] = (seed + i * 7919) % 65535  # 7919 is prime — spread

    # Build program from weights as memory values + fixed emit pattern
    # Load weights into mem[256..] — safe above SUBLEQ program area
    prog = []
    W_OFFSET = 256
    n_w = min(len(weights), 1000)
    for i, w in enumerate(weights[:n_w]):
        m.mem[W_OFFSET + i] = w

    # Program: for each pair of weight addresses, SUBLEQ then emit
    for i in range(0, n_w - 1, 2):
        A = W_OFFSET + i
        B = W_OFFSET + i + 1
        next_pc = len(prog) + 6  # next instruction after this + emit
        emit_pc = len(prog) + 3  # emit is right after this subleq
        prog.extend([A, B, emit_pc])     # SUBLEQ — if M[B]<=0 jump to emit
        prog.extend([-1, B, next_pc])    # EMIT M[B]

    prog.extend([-1, -1, -1])  # HALT
    m.load(prog)
    return m.run(max_steps)

def born_collapse(agent_results, n):
    """Phi-weighted collapse — same as quantum-swarm qNormalize + qMeasure."""
    all_vals = []
    for i, (name, out) in enumerate(agent_results):
        phi_w = PHI ** (-i)
        for v in out[:10]:
            all_vals.append(int(v * phi_w))
    if not all_vals:
        return 0
    # Born rule: sum of phi-weighted amplitudes
    return sum(all_vals) % 65535

def run_swarm(n=8):
    print(f'\n=== QUANTUM SWARM — {n} agents ===')

    # 1. ANU vacuum entropy
    vacuum = anu_entropy(32)
    print(f'vacuum: {vacuum.hex()[:16]}...')

    # 2. Master seed
    master = hmac.new(vacuum, b'sovereign-swarm-master', hashlib.sha256).digest()

    # 3. Agent seeds
    seeds = derive_seeds(master, n)
    print(f'seeds derived: {n}')

    # 4. Load collectivekitty weights
    weights = load_weights(max_values=3000)
    print(f'weights loaded: {len(weights)}')

    # 5. Assign agents to phi-slices
    slice_names = list(AGENT_SLICES.keys())
    agent_assignments = []
    for i in range(n):
        name = slice_names[i % len(slice_names)]
        agent_assignments.append((i, name, seeds[i]))

    # 6. Run all agents concurrently
    results = []
    lock = threading.Lock()

    def run(i, name, seed):
        out = run_agent(name, seed, weights)
        with lock:
            results.append((i, name, out))
        print(f'  agent-{i} [{name}] -> {len(out)} outputs, sample: {out[:3]}')

    threads = [threading.Thread(target=run, args=(i, n, s)) for i, n, s in agent_assignments]
    for t in threads: t.start()
    for t in threads: t.join()

    results.sort(key=lambda x: x[0])

    # 7. Born-collapse
    named_results = [(r[1], r[2]) for r in results]
    answer = born_collapse(named_results, n)

    print(f'\nBorn-collapse answer: {answer}')
    print(f'As Abjad: {answer % 1000}')
    return answer

if __name__ == '__main__':
    import sys
    n = int(sys.argv[1]) if len(sys.argv) > 1 else 8
    run_swarm(n)
