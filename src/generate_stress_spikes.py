import os
import json
import time
import random
import sys

STATE_MATRIX_PATH = "workspace/Metastasis-Tracker-AI/src/data/state_matrix.json"

def inject_stress_spikes(cycles=30, spike_amplitude=2200, nominal_baseline=425):
    """
    Simulates a volatile workload profile by writing artificial execution times
    directly into the state matrix to test compiler feedback loops.
    """
    print("[*] Starting Hardware Stress Spike Simulation Engine...")
    os.makedirs(os.path.dirname(STATE_MATRIX_PATH), exist_ok=True)

    # Core stress wave profile: High stress phase followed by recovery phase
    for cycle in range(1, cycles + 1):
        if not os.path.exists(STATE_MATRIX_PATH):
            print("[!] Target state matrix file missing. Waiting for compiler initialization...")
            time.sleep(1)
            continue

        try:
            with open(STATE_MATRIX_PATH, 'r') as f:
                data = json.load(f)
        except (json.JSONDecodeError, IOError):
            time.sleep(0.1) # File lock avoidance buffer
            continue

        # Generate a dynamic stress timeline curve
        if cycle <= 12:
            # Phase 1: Heavy rendering overload spike simulation
            simulated_time = random.randint(spike_amplitude - 200, spike_amplitude + 300)
            print(f"[!] [Cycle {cycle}/{cycles}] Injecting STRESS OVERLOAD: {simulated_time} us")
        else:
            # Phase 2: System recovery / thermal dissipation phase
            simulated_time = max(nominal_baseline, nominal_baseline + (cycles - cycle) * 30 + random.randint(-50, 50))
            print(f"[*] [Cycle {cycle}/{cycles}] Injecting HEALING COOL-DOWN: {simulated_time} us")

        # Overwrite the target telemetric register
        if "gpu_telemetry_profile" not in data:
            data["gpu_telemetry_profile"] = {}
        data["gpu_telemetry_profile"]["last_measured_execution_time"] = simulated_time

        with open(STATE_MATRIX_PATH, 'w') as f:
            json.dump(data, f, indent=2)

        # Pause to let the compiler loop read the parameters and apply downscaling corrections
        time.sleep(0.8)

if __name__ == "__main__":
    # Ingest runtime configuration lengths from the shell script wrapper if present
    iterations = int(sys.argv[1]) if len(sys.argv) > 1 else 30
    inject_stress_spikes(cycles=iterations)
