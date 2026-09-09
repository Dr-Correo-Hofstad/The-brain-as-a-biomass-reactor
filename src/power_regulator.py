import time
import math

class BioElectricConverter:
    def __init__(self, target_output_v=5.0, efficiency=0.92):
        self.target_output_v = target_output_v
        self.efficiency = efficiency
        self.accumulated_energy_joules = 0.0
        self.rail_voltage = 0.0

    def process_haptic_ac_input(self, input_amplitude_v=0.290, frequency_hz=100.0, duration_sec=1.0, sampling_rate_hz=1000):
        """
        Rectifies and steps up the 290mV, 100Hz biological AC voltage waveform
        into a stabilized digital power bus.
        """
        total_samples = int(duration_sec * sampling_rate_hz)
        time_step = 1.0 / sampling_rate_hz
        rectified_joules = 0.0
        
        # Load simulation impedance (e.g., equivalent tissue/tactile boundary resistance)
        r_load = 500.0 
        
        for sample in range(total_samples):
            t = sample * time_step
            # Pure sinusoidal AC input voltage mapping the biological haptic baseline
            v_ac = input_amplitude_v * math.sin(2 * math.pi * frequency_hz * t)
            
            # Full-wave synchronous rectification simulation
            v_rectified = abs(v_ac)
            
            # Instantaneous power calculation: P = V^2 / R
            p_inst = (v_rectified ** 2) / r_load
            rectified_joules += p_inst * time_step * self.efficiency

        self.accumulated_energy_joules += rectified_joules
        # Update output bus rail voltage based on typical capacitance load equation: V = sqrt(2E/C)
        c_storage = 0.022  # 22 milliFarad storage capacitor bank
        self.rail_voltage = math.sqrt((2 * self.accumulated_energy_joules) / c_storage)
        if self.rail_voltage > self.target_output_v:
            self.rail_voltage = self.target_output_v  # Clamped by zener diode simulation
            
        print(f"[+] Haptic AC Conversion Cycle Complete.")
        print(f"    - Input Profile: {input_amplitude_v*1000:.1f}mV @ {frequency_hz}Hz")
        print(f"    - Energy Harvested: {rectified_joules:.6f} Joules")
        print(f"    - Bus Rail Voltage: {self.rail_voltage:.4f}V DC Stabilized\n")

if __name__ == "__main__":
    regulator = BioElectricConverter(target_output_v=3.3)
    regulator.process_haptic_ac_input(duration_sec=2.5)
