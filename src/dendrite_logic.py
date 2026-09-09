import sys

class DendriteGateArray:
    def __init__(self):
        # Maps unique condition tags to confidence coefficient parameters
        self.gates = {
            "G_APPROACH_EXTEND": 0.10,
            "G_APPROACH_STEP_EXTEND": 0.15,
            "G_APPROACH_EXTEND_CLOSE": 0.20
        }
        
    def execute_logic_step(self, throw_index, ball_distance, match_timing):
        """Processes conditional execution path and evaluates response output."""
        print(f"--- Processing Input Frame Throw #{throw_index} ---")
        
        # Primary gating evaluations based on conditional branches
        if ball_distance > 10 and match_timing < 0.5:
            selected_path = "G_APPROACH_EXTEND"
            outcome = "MISSED"
        elif ball_distance <= 10 and match_timing < 0.8:
            selected_path = "G_APPROACH_STEP_EXTEND"
            outcome = "BOUNCED_OUT"
        else:
            selected_path = "G_APPROACH_EXTEND_CLOSE"
            outcome = "CAUGHT"
            
        self.apply_reinforcement(selected_path, outcome)
        
    def apply_reinforcement(self, path, outcome):
        current_weight = self.gates[path]
        if outcome == "CAUGHT":
            # Success reinforcement: increase routing path lock-in
            reward = 0.40
            self.gates[path] = min(1.0, current_weight + reward)
            print(f"[SUCCESS] Array Gated: {path} | Logic Match verified. Confidence augmented.")
        else:
            # Error degradation: decrease dependency on unoptimized pathing
            penalty = 0.05
            self.gates[path] = max(0.01, current_weight - penalty)
            print(f"[FAILURE] Array Gated: {path} | Output error. Confidence degraded.")
            
        print(f"    - Current Path Confidence Matrix: {self.gates[path]:.4f}\n")

if __name__ == "__main__":
    net_array = DendriteGateArray()
    # Sequence mapping the historical training profile of the controller matrix
    net_array.execute_logic_step(throw_index=1, ball_distance=15, match_timing=0.3)
    net_array.execute_logic_step(throw_index=2, ball_distance=8,  match_timing=0.6)
    net_array.execute_logic_step(throw_index=3, ball_distance=4,  match_timing=0.95)
