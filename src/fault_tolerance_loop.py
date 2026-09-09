import sys
import time

class SystemSupervisor:
    def __init__(self):
        self.system_stable = True
        self.error_count = 0
        self.registry_state = "NOMINAL_0x00"

    def execute_instruction_pipeline(self, target_data_packet):
        """Processes runtime configurations while evaluating data integrity."""
        try:
            print(f"[*] Ingesting Signal Packet: {target_data_packet['id']}")
            
            # Simulate a memory tracking fault condition
            if target_data_packet["checksum"] != sum(target_data_packet["data"]):
                raise ValueError("REGISTRY_CORRUPTION_DETECTED")
                
            # Simulate a processing threshold exception
            if target_data_packet["thermal_load"] > 85.0:
                raise RuntimeError("THERMAL_OVERLOAD_WARN")
                
            self.registry_state = f"UPDATED_{hex(sum(target_data_packet['data']))}"
            print(f"    - System Core Executed Successfully. Current Register: {self.registry_state}")
            
        except (ValueError, RuntimeError) as system_fault:
            self.execute_soft_exception_recovery(system_fault)

    def execute_soft_exception_recovery(self, fault_type):
        """Fault-tolerance protocol handler. Performs targeted register clears."""
        self.error_count += 1
        print(f"\n[!] WARNING: System Fault Triggered: {fault_type}")
        print(f"    - Initiating Soft Exception Recovery Routine...")
        
        if str(fault_type) == "REGISTRY_CORRUPTION_DETECTED":
            print("    - Clearing corrupted volatile registers...")
            print("    - Performing data roll-back to last known-good checkpoint state.")
            self.registry_state = "RESET_SAFE_0x00"
        elif str(fault_type) == "THERMAL_OVERLOAD_WARN":
            print("    - Throttling computing arrays by 50%.")
            print("    - Diverting fluid cooling lines to high-load sectors.")
            
        print(f"    - Recovery Complete. System Status: STABLE | Total Mitigated Faults: {self.error_count}\n")

if __name__ == "__main__":
    supervisor = SystemSupervisor()
    
    # Valid payload test
    good_packet = {"id": "PKT_001", "data":, "checksum": 60, "thermal_load": 32.5}
    supervisor.execute_instruction_pipeline(good_packet)
    
    # Corrupted payload test (Triggers soft-reset recovery protocol)
    corrupted_packet = {"id": "PKT_002", "data":, "checksum": 999, "thermal_load": 40.0}
    supervisor.execute_instruction_pipeline(corrupted_packet)
