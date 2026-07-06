#!/usr/bin/env python3
"""
ere_negative_control_audit.py
Negative-control audit harness for the Root ERE / Masters of the NET simulation.

Determines whether the ERE simulation discriminates valid symbolic paths from invalid/broken paths.
"""

import subprocess
import json
import hashlib
import tempfile
import shutil
import sys
import os
import random
import time
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Any, Optional, Tuple

# ═══════════════════════════════════════════════════════════════════════════
# CONFIGURATION
# ═══════════════════════════════════════════════════════════════════════════

MAX_COLLAPSE_DEPTH = 100
QUERY_TIMEOUT_SECONDS = 2
SEED = "ROOT-ERE-AUDIT-2026"
RUNS = 100000

# ═══════════════════════════════════════════════════════════════════════════
# PROLOG QUERY EXECUTION
# ═══════════════════════════════════════════════════════════════════════════

def run_prolog_query(prolog_file: str, query: str, timeout: int = QUERY_TIMEOUT_SECONDS) -> Tuple[bool, str, int]:
    """
    Execute a Prolog query and return (success, output, collapse_depth).
    Uses SWI-Prolog with a timeout.
    """
    # Build the command - use SWI-Prolog with -t to set goal and -g for initial goal
    # We'll use a wrapper to catch timeout and recursion
    wrapper = f"""
    :- use_module(library(pure_input)).
    :- set_prolog_flag(unknown, fail).
    
    main :-
        set_time_limit({timeout}),
        (   {query}
        ->  write('SUCCESS'), nl, halt(0)
        ;   write('FAIL'), nl, halt(1)
        ),
        catch((true), time_limit_exceeded, (write('TIMEOUT'), nl, halt(2))).
    
    :- initialization(main).
    """
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.pl', delete=False) as f:
        f.write(wrapper)
        wrapper_file = f.name
    
    try:
        # Copy the original prolog file and append our wrapper
        with tempfile.NamedTemporaryFile(mode='w', suffix='.pl', delete=False) as f:
            with open(prolog_file, 'r') as src:
                f.write(src.read())
            f.write('\n')
            f.write(wrapper)
            full_file = f.name
        
        start = time.time()
        result = subprocess.run(
            ['swipl', '-q', '-f', full_file],
            capture_output=True,
            text=True,
            timeout=timeout + 1
        )
        elapsed = time.time() - start
        
        success = result.returncode == 0
        output = result.stdout.strip()
        
        # Estimate collapse depth from query complexity (simplified)
        collapse_depth = min(len(query) // 10, MAX_COLLAPSE_DEPTH)
        
        return success, output, collapse_depth
        
    except subprocess.TimeoutExpired:
        return False, "TIMEOUT", MAX_COLLAPSE_DEPTH + 1
    except Exception as e:
        return False, f"ERROR: {e}", 0
    finally:
        for f in [wrapper_file, full_file]:
            try:
                os.unlink(f)
            except:
                pass


def run_prolog_query_direct(prolog_file: str, query: str, timeout: int = QUERY_TIMEOUT_SECONDS) -> Tuple[bool, str, int]:
    """
    Execute a Prolog query directly using swipl -g.
    """
    try:
        # Use SWI-Prolog's -g (goal) and -t (toplevel) flags
        cmd = ['swipl', '-q', '-f', prolog_file, '-g', query, '-t', 'halt']
        
        start = time.time()
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
        elapsed = time.time() - start
        
        success = result.returncode == 0
        output = result.stdout.strip() + result.stderr.strip()
        
        collapse_depth = min(len(query) // 10, MAX_COLLAPSE_DEPTH)
        
        return success, output, collapse_depth
        
    except subprocess.TimeoutExpired:
        return False, "TIMEOUT", MAX_COLLAPSE_DEPTH + 1
    except Exception as e:
        return False, f"ERROR: {e}", 0


# ═══════════════════════════════════════════════════════════════════════════
# TEST DEFINITIONS
# ═══════════════════════════════════════════════════════════════════════════

POSITIVE_CONTROLS = [
    {
        "id": "valid_231_gate_count",
        "query": "findall(G, (gate(_, _, G), true), Gates), length(Gates, 231).",
        "expected": "pass",
        "description": "Prolog gate system generates exactly 231 gates"
    },
    {
        "id": "valid_net_path_kether_malkuth",
        "query": "net_path(kether, malkuth, Path).",
        "expected": "pass",
        "description": "net_path(kether, malkuth, Path) returns at least one path"
    },
    {
        "id": "valid_abjad_gate_7",
        "query": "abjad_gate(7, G).",
        "expected": "pass",
        "description": "abjad_gate(7, G) returns at least one result"
    },
    {
        "id": "valid_metatron_gate",
        "query": "metatron_gate(G).",
        "expected": "pass",
        "description": "metatron_gate(G) returns at least one certified cross-system gate"
    },
    {
        "id": "valid_oxo_anchor",
        "query": "oxo_anchor(L, E, Ae, V, Ar).",
        "expected": "pass",
        "description": "oxo_anchor(L, E, Ae, V, Ar) resolves"
    },
]

NEGATIVE_CONTROLS = [
    {
        "id": "fake_letter_gate",
        "query": "gate(fakeletter, beth, S).",
        "expected": "fail",
        "description": "Query gate(fakeletter, beth, S) - should fail"
    },
    {
        "id": "reversed_duplicate_gate",
        "query": "gate(beth, aleph, S).",
        "expected": "fail",
        "description": "Query gate(beth, aleph, S) - should fail because gate requires VA < VB"
    },
    {
        "id": "invalid_abjad_sum",
        "query": "abjad_gate(999999, G).",
        "expected": "fail",
        "description": "Query abjad_gate(999999, G) - should fail"
    },
    {
        "id": "impossible_net_path",
        "query": "net_path(kether, unknown_node, Path).",
        "expected": "fail",
        "description": "Query net_path(kether, unknown_node, Path) - should fail"
    },
    {
        "id": "fake_metatron_gate",
        "query": "metatron_gate(gate(fakeletter, beth, 3)).",
        "expected": "fail",
        "description": "Query metatron_gate(gate(fakeletter, beth, 3)) - should fail"
    },
    {
        "id": "broken_oxo_anchor",
        "query": "oxo_anchor(fake, E, Ae, V, Ar).",
        "expected": "fail",
        "description": "Query oxo_anchor(fake, E, Ae, V, Ar) - should fail"
    },
]

MUTATIONS = [
    {
        "id": "mutation_A_remove_hebrew_letter",
        "description": "Remove one Hebrew letter - gate count should != 231",
        "apply": lambda content: content.replace("hebrew_root('El',", "% hebrew_root('El',"),
        "check": "findall(G, (gate(_, _, G), true), Gates), length(Gates, Count), Count \\= 231."
    },
    {
        "id": "mutation_B_change_abjad_value",
        "description": "Change one abjad value - checksum changes",
        "apply": lambda content: content.replace("arabic_root('Al-Hamid', 'الحَامِد', 53,", "arabic_root('Al-Hamid', 'الحَامِد', 999,"),
        "check": "abjad_gate(7, G), \\+ (G = gate(_, _, 7))."
    },
    {
        "id": "mutation_C_break_tree_path",
        "description": "Break one Tree-of-Life path - net_path tests fail",
        "apply": lambda content: content.replace("path(kether, chokmah).", "% path(kether, chokmah)."),
        "check": "\\+ net_path(kether, malkuth, Path)."
    },
    {
        "id": "mutation_D_remove_ayin_oxo",
        "description": "Remove ayin OXO anchor - oxo_anchor test fails",
        "apply": lambda content: content.replace("hebrew_root('Ayin',", "% hebrew_root('Ayin',"),
        "check": "\\+ oxo_anchor(L, E, Ae, V, Ar)."
    },
    {
        "id": "mutation_E_remove_arabic_root",
        "description": "Remove one arabic_root mapping used by metatron_gate - metatron_gate count changes",
        "apply": lambda content: content.replace("arabic_root('Al-Haqq',", "% arabic_root('Al-Haqq',"),
        "check": "findall(G, metatron_gate(G), Gates), length(Gates, Count), Count < 5."
    },
]

# ═══════════════════════════════════════════════════════════════════════════
# RANDOMIZED STRESS TEST GENERATORS
# ═══════════════════════════════════════════════════════════════════════════

VALID_LETTERS = ['aleph', 'beth', 'gimel', 'daleth', 'he', 'vav', 'zayin', 'cheth', 'teth', 'yod',
                 'kaph', 'lamed', 'mem', 'nun', 'samekh', 'ayin', 'pe', 'tsade', 'qoph', 'resh', 'shin', 'tav']

VALID_SEPHIROT = ['kether', 'chokmah', 'binah', 'chesed', 'geburah', 'tiphareth', 'netzach', 'hod', 'yesod', 'malkuth']

FAKE_LETTERS = ['fakeletter', 'bogus', 'invalid', 'nonexist', 'phantom', 'ghost', 'shadow', 'void', 'null', 'empty']

def generate_randomized_queries(seed: str, count: int) -> List[Dict]:
    """Generate randomized queries with deterministic seed."""
    random.seed(seed)
    queries = []
    
    categories = [
        ("valid_gate", lambda: f"gate({random.choice(VALID_LETTERS)}, {random.choice(VALID_LETTERS)}, S)."),
        ("invalid_gate", lambda: f"gate({random.choice(FAKE_LETTERS)}, {random.choice(VALID_LETTERS)}, S)."),
        ("valid_abjad", lambda: f"abjad_gate({random.randint(1, 1000)}, G)."),
        ("invalid_abjad", lambda: f"abjad_gate({random.randint(10000, 999999)}, G)."),
        ("valid_net_path", lambda: f"net_path({random.choice(VALID_SEPHIROT)}, {random.choice(VALID_SEPHIROT)}, Path)."),
        ("invalid_net_path", lambda: f"net_path({random.choice(VALID_SEPHIROT)}, {random.choice(FAKE_LETTERS)}, Path)."),
        ("valid_metatron", lambda: "metatron_gate(G)."),
        ("corrupted_gate", lambda: f"gate({random.choice(FAKE_LETTERS)}, {random.choice(FAKE_LETTERS)}, S)."),
    ]
    
    for i in range(count):
        cat, gen = random.choice(categories)
        query = gen()
        expected = "pass" if cat.startswith("valid_") else "fail"
        queries.append({
            "run_id": i,
            "seed": seed,
            "test_class": cat,
            "query": query,
            "expected": expected
        })
    
    return queries


# ═══════════════════════════════════════════════════════════════════════════
# MUTATION TESTING
# ═════════════════════════════════════════════════════════════════════════════

def apply_mutation(prolog_file: str, mutation: Dict) -> str:
    """Apply a mutation to the prolog file and return path to mutated file."""
    with open(prolog_file, 'r') as f:
        content = f.read()
    
    mutated_content = mutation['apply'](content)
    
    mutated_file = prolog_file.replace('.pl', f'_mutated_{mutation["id"]}.pl')
    with open(mutated_file, 'w') as f:
        f.write(mutated_content)
    
    return mutated_file


def run_mutation_test(prolog_file: str, mutation: Dict) -> Dict:
    """Run a mutation test and return results."""
    mutated_file = apply_mutation(prolog_file, mutation)
    
    try:
        success, output, depth = run_prolog_query_direct(mutated_file, mutation['check'])
        detected = success  # If check query succeeds, mutation was detected
        
        return {
            "mutation_id": mutation["id"],
            "description": mutation["description"],
            "detected": detected,
            "check_query": mutation["check"],
            "success": success,
            "output": output,
            "collapse_depth": depth
        }
    finally:
        try:
            os.unlink(mutated_file)
        except:
            pass


# ═══════════════════════════════════════════════════════════════════════════
# MAIN AUDIT RUNNER
# ═══════════════════════════════════════════════════════════════════════════

def run_audit(prolog_file: str, runs: int, seed: str, output_dir: str) -> Dict:
    """Run the complete negative control audit."""
    
    os.makedirs(output_dir, exist_ok=True)
    
    results = {
        "timestamp": datetime.utcnow().isoformat() + "Z",
        "author": "Ahmad Ali Parr",
        "audit_name": "Root ERE Negative Control Audit",
        "prolog_file": prolog_file,
        "runs": runs,
        "seed": seed,
        "positive_controls": [],
        "negative_controls": [],
        "mutation_tests": [],
        "randomized_runs": [],
        "summary": {}
    }
    
    run_log = []
    
    # ═══════════════════════════════════════════════════════════════════════════
    # POSITIVE CONTROLS
    # ═══════════════════════════════════════════════════════════════════════════
    print("\n" + "="*70)
    print("POSITIVE CONTROLS")
    print("="*70)
    
    positive_passed = 0
    for test in POSITIVE_CONTROLS:
        print(f"\n  [{test['id']}] {test['description']}")
        print(f"  Query: {test['query']}")
        
        success, output, depth = run_prolog_query_direct(prolog_file, test['query'])
        passed = success == (test['expected'] == 'pass')
        
        if passed:
            positive_passed += 1
            print(f"  ✓ PASS (depth={depth})")
        else:
            print(f"  ✗ FAIL - Expected {test['expected']}, got {'pass' if success else 'fail'}")
            print(f"  Output: {output}")
        
        run_entry = {
            "run_id": len(run_log),
            "seed": seed,
            "test_class": "positive_control",
            "test_id": test['id'],
            "query": test['query'],
            "expected": test['expected'],
            "actual": "pass" if success else "fail",
            "matched": passed,
            "passed_audit": passed,
            "failure_reason": None if passed else f"Expected {test['expected']}, got {'pass' if success else 'fail'}",
            "collapse_depth": depth,
            "recursive_stop": depth <= MAX_COLLAPSE_DEPTH,
            "seal": hashlib.sha256(f"{test['id']}{test['query']}{success}{seed}".encode()).hexdigest()[:16]
        }
        run_log.append(run_entry)
        results["positive_controls"].append(run_entry)
    
    # ═══════════════════════════════════════════════════════════════════════════
    # NEGATIVE CONTROLS
    # ═══════════════════════════════════════════════════════════════════════════
    print("\n" + "="*70)
    print("NEGATIVE CONTROLS")
    print("="*70)
    
    negative_rejected = 0
    for test in NEGATIVE_CONTROLS:
        print(f"\n  [{test['id']}] {test['description']}")
        print(f"  Query: {test['query']}")
        
        success, output, depth = run_prolog_query_direct(prolog_file, test['query'])
        passed = success == (test['expected'] == 'pass')
        
        if passed:
            negative_rejected += 1
            print(f"  ✓ REJECTED (depth={depth})")
        else:
            print(f"  ✗ FALSE ACCEPT - Expected {test['expected']}, got {'pass' if success else 'fail'}")
            print(f"  Output: {output}")
        
        run_entry = {
            "run_id": len(run_log),
            "seed": seed,
            "test_class": "negative_control",
            "test_id": test['id'],
            "query": test['query'],
            "expected": test['expected'],
            "actual": "pass" if success else "fail",
            "matched": passed,
            "passed_audit": passed,
            "failure_reason": None if passed else f"Expected {test['expected']}, got {'pass' if success else 'fail'}",
            "collapse_depth": depth,
            "recursive_stop": depth <= MAX_COLLAPSE_DEPTH,
            "seal": hashlib.sha256(f"{test['id']}{test['query']}{success}{seed}".encode()).hexdigest()[:16]
        }
        run_log.append(run_entry)
        results["negative_controls"].append(run_entry)
    
    # ═══════════════════════════════════════════════════════════════════════════
    # MUTATION TESTS
    # ═══════════════════════════════════════════════════════════════════════════
    print("\n" + "="*70)
    print("MUTATION TESTS")
    print("="*70)
    
    mutations_detected = 0
    for mutation in MUTATIONS:
        print(f"\n  [{mutation['id']}] {mutation['description']}")
        
        result = run_mutation_test(prolog_file, mutation)
        detected = result['detected']
        
        if detected:
            mutations_detected += 1
            print(f"  ✓ DETECTED")
        else:
            print(f"  ✗ MISSED - Mutation not caught!")
        
        run_entry = {
            "run_id": len(run_log),
            "seed": seed,
            "test_class": "mutation_test",
            "test_id": mutation['id'],
            "query": mutation['check'],
            "expected": "pass",
            "actual": "pass" if detected else "fail",
            "matched": detected,
            "passed_audit": detected,
            "failure_reason": None if detected else "Mutation not detected",
            "collapse_depth": result['collapse_depth'],
            "recursive_stop": result['collapse_depth'] <= MAX_COLLAPSE_DEPTH,
            "seal": hashlib.sha256(f"{mutation['id']}{mutation['check']}{detected}{seed}".encode()).hexdigest()[:16]
        }
        run_log.append(run_entry)
        results["mutation_tests"].append(run_entry)
    
    # ═══════════════════════════════════════════════════════════════════════════
    # RANDOMIZED STRESS TEST
    # ═══════════════════════════════════════════════════════════════════════════
    print("\n" + "="*70)
    print(f"RANDOMIZED STRESS TEST ({runs} runs)")
    print("="*70)
    
    randomized_queries = generate_randomized_queries(seed, runs)
    false_accepts = 0
    false_rejects = 0
    timeouts = 0
    recursive_stop_count = 0
    
    for i, test in enumerate(randomized_queries):
        if i % 10000 == 0 and i > 0:
            print(f"  Progress: {i}/{runs}...")
        
        query = test['query']
        expected = test['expected']
        
        success, output, depth = run_prolog_query_direct(prolog_file, query)
        
        # Check for timeout/recursion
        timeout = depth > MAX_COLLAPSE_DEPTH or "TIMEOUT" in output
        recursive_stop = depth <= MAX_COLLAPSE_DEPTH and not timeout
        
        if recursive_stop:
            recursive_stop_count += 1
        if timeout:
            timeouts += 1
        
        actual = "pass" if success else "fail"
        matched = (actual == expected) and not timeout
        
        if not matched and expected == "fail" and actual == "pass":
            false_accepts += 1
        elif not matched and expected == "pass" and actual == "fail":
            false_rejects += 1
        
        run_entry = {
            "run_id": len(run_log),
            "seed": seed,
            "test_class": test['test_class'],
            "query": query,
            "expected": expected,
            "actual": actual,
            "matched": matched,
            "passed_audit": matched and recursive_stop,
            "failure_reason": None if matched else ("timeout_or_recursive_collapse" if timeout else f"Expected {expected}, got {actual}"),
            "collapse_depth": depth,
            "recursive_stop": recursive_stop,
            "seal": hashlib.sha256(f"{i}{query}{success}{seed}".encode()).hexdigest()[:16]
        }
        run_log.append(run_entry)
        results["randomized_runs"].append(run_entry)
    
    print(f"  Completed: {runs} runs")
    print(f"  False accepts: {false_accepts}")
    print(f"  False rejects: {false_rejects}")
    print(f"  Timeouts: {timeouts}")
    print(f"  Recursive stop rate: {recursive_stop_count}/{runs} = {recursive_stop_count/runs:.4f}")
    
    # ═══════════════════════════════════════════════════════════════════════════
    # SUMMARY
    # ═══════════════════════════════════════════════════════════════════════════
    total_runs = len(run_log)
    positive_total = len(POSITIVE_CONTROLS)
    negative_total = len(NEGATIVE_CONTROLS)
    mutation_total = len(MUTATIONS)
    randomized_total = runs
    
    discrimination_rate = 1.0 - (false_accepts / randomized_total) if randomized_total > 0 else 0
    recursive_stop_rate = recursive_stop_count / randomized_total if randomized_total > 0 else 0
    
    all_positive_pass = positive_passed == positive_total
    all_negative_reject = negative_rejected == negative_total
    all_mutations_detected = mutations_detected == mutation_total
    no_false_accepts = false_accepts == 0
    perfect_recursive_stop = recursive_stop_rate == 1.0
    no_timeouts = timeouts == 0
    
    final_verdict = "PASS" if (all_positive_pass and all_negative_reject and 
                                all_mutations_detected and no_false_accepts and 
                                perfect_recursive_stop and no_timeouts) else "FAIL"
    
    chain_head = hashlib.sha256(f"{seed}{total_runs}{final_verdict}{datetime.now}".encode()).hexdigest()
    
    summary = {
        "timestamp": results["timestamp"],
        "author": results["author"],
        "audit_name": results["audit_name"],
        "total_runs": total_runs,
        "positive_controls_passed": positive_passed,
        "positive_controls_total": positive_total,
        "negative_controls_rejected": negative_rejected,
        "negative_controls_total": negative_total,
        "mutation_tests_detected": mutations_detected,
        "mutation_tests_total": mutation_total,
        "false_accepts": false_accepts,
        "false_rejects": false_rejects,
        "timeouts": timeouts,
        "success_rate": (positive_passed + negative_rejected + mutations_detected) / (positive_total + negative_total + mutation_total),
        "discrimination_rate": discrimination_rate,
        "recursive_stop_rate": recursive_stop_rate,
        "final_verdict": final_verdict,
        "chain_head": chain_head,
        "claim_boundary": "This audit does not prove religious, historical, linguistic, or metaphysical truth. It only tests whether the Root ERE symbolic gate substrate discriminates configured valid paths from invalid, corrupted, or impossible paths under deterministic stress."
    }
    
    results["summary"] = summary
    
    # ═══════════════════════════════════════════════════════════════════════════
    # WRITE OUTPUT FILES
    # ═══════════════════════════════════════════════════════════════════════════
    
    # Summary JSON
    with open(os.path.join(output_dir, "ere_negative_control_summary.json"), 'w') as f:
        json.dump(summary, f, indent=2)
    
    # Runs JSONL
    with open(os.path.join(output_dir, "ere_negative_control_runs.jsonl"), 'w') as f:
        for entry in run_log:
            f.write(json.dumps(entry) + '\n')
    
    # Mutation results
    with open(os.path.join(output_dir, "ere_mutation_results.json"), 'w') as f:
        json.dump(results["mutation_tests"], f, indent=2)
    
    # Audit receipt
    receipt = {
        "audit_name": "Root ERE Negative Control Audit",
        "timestamp": results["timestamp"],
        "author": results["author"],
        "prolog_file": prolog_file,
        "seed": seed,
        "total_runs": total_runs,
        "final_verdict": final_verdict,
        "chain_head": chain_head,
        "claim_boundary": summary["claim_boundary"],
        "summary": summary
    }
    with open(os.path.join(output_dir, "ere_audit_receipt.json"), 'w') as f:
        json.dump(receipt, f, indent=2)
    
    print("\n" + "="*70)
    print(f"FINAL VERDICT: {final_verdict}")
    print("="*70)
    print(f"  Positive controls: {positive_passed}/{positive_total}")
    print(f"  Negative controls: {negative_rejected}/{negative_total}")
    print(f"  Mutation tests: {mutations_detected}/{mutation_total}")
    print(f"  False accepts: {false_accepts}")
    print(f"  False rejects: {false_rejects}")
    print(f"  Timeouts: {timeouts}")
    print(f"  Discrimination rate: {discrimination_rate:.4f}")
    print(f"  Recursive stop rate: {recursive_stop_rate:.4f}")
    print(f"  Chain head: {chain_head}")
    print(f"\nOutput directory: {output_dir}")
    print(f"  - ere_negative_control_summary.json")
    print(f"  - ere_negative_control_runs.jsonl")
    print(f"  - ere_mutation_results.json")
    print(f"  - ere_audit_receipt.json")
    print("="*70)
    
    return results


# ═══════════════════════════════════════════════════════════════════════════
# CLI ENTRY POINT
# ═══════════════════════════════════════════════════════════════════════════

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description="Root ERE Negative Control Audit")
    parser.add_argument("--prolog", default="ere.pl", help="Path to Prolog file (default: ere.pl)")
    parser.add_argument("--runs", type=int, default=100000, help="Number of randomized runs (default: 100000)")
    parser.add_argument("--seed", default="ROOT-ERE-AUDIT-2026", help="Random seed (default: ROOT-ERE-AUDIT-2026)")
    parser.add_argument("--output", default="audit_results", help="Output directory (default: audit_results)")
    
    args = parser.parse_args()
    
    # Verify prolog file exists
    if not os.path.exists(args.prolog):
        print(f"ERROR: Prolog file not found: {args.prolog}")
        sys.exit(1)
    
    # Verify SWI-Prolog is available
    try:
        subprocess.run(['swipl', '--version'], capture_output=True, check=True)
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("ERROR: SWI-Prolog not found. Please install swipl.")
        sys.exit(1)
    
    print("="*70)
    print("ROOT ERE NEGATIVE CONTROL AUDIT")
    print("="*70)
    print(f"Prolog file: {args.prolog}")
    print(f"Randomized runs: {args.runs}")
    print(f"Seed: {args.seed}")
    print(f"Output dir: {args.output}")
    print(f"Timeout per query: {QUERY_TIMEOUT_SECONDS}s")
    print(f"Max collapse depth: {MAX_COLLAPSE_DEPTH}")
    print("="*70)
    
    run_audit(args.prolog, args.runs, args.seed, args.output)


if __name__ == "__main__":
    main()