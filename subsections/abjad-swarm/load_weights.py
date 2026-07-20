#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# load_weights.py
# Loads collectivekitty LoRA checkpoint-186 into flat int list for SUBLEQ

from safetensors import safe_open
import struct

CKPT = 'C:/Users/jessi/Desktop/bobs control repo/DEVFLOW-FINANCE/collectivekitty-ml/adapters/checkpoints/checkpoint-186/adapter_model.safetensors'

def load_weights(max_values=30000):
    """
    Load all LoRA weights, flatten to int addresses for SUBLEQ.
    Float32 -> scale to [1, 32767] address range.
    """
    raw = []
    with safe_open(CKPT, framework='pt', device='cpu') as f:
        for key in f.keys():
            t = f.get_tensor(key).float()
            raw.extend(t.flatten().tolist())
            if len(raw) >= max_values:
                break

    raw = raw[:max_values]

    # Find min/max for normalization
    mn = min(raw)
    mx = max(raw)
    span = mx - mn if mx != mn else 1.0

    # Scale to [1, 32767] — valid SUBLEQ address space
    scaled = [max(1, int(((v - mn) / span) * 32766) + 1) for v in raw]
    return scaled

if __name__ == '__main__':
    w = load_weights()
    print(f'loaded {len(w)} weights')
    print(f'sample: {w[:8]}')
    print(f'min: {min(w)}  max: {max(w)}')
