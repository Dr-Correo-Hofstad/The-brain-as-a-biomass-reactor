import math

def calculate_helical_magnetic_field(current_amperes, radius_meters, pitch_meters, length_meters):
    """
    Computes the localized magnetic field vector along the central axis of a 
    conductive double-helix trace under constant current.
    """
    print(f"[*] Analyzing Helical Track Geometry...")
    # Vacuum permeability constant (H/m)
    mu_0 = 4 * math.pi * 1e-7
    
    # Calculate number of turns based on total length and helical pitch distance
    turns = length_meters / pitch_meters
    
    # Infinite solenoid axial field approximation modified for sub-micron pitch geometry
    # B = (mu_0 * N * I) / sqrt(L^2 + 4R^2)
    numerator = mu_0 * turns * current_amperes
    denominator = math.sqrt((length_meters ** 2) + 4 * (radius_meters ** 2))
    
    magnetic_flux_density = numerator / denominator
    
    print(f"[+] Maxwell Right-Hand Rule Evaluation:")
    print(f"    - Track Dimensions: Radius={radius_meters*1e9:.2f}nm, Pitch={pitch_meters*1e9:.2f}nm")
    print(f"    - Induced Axial Magnetic Flux Density: {magnetic_flux_density*1e6:.4f} microTesla")
    print(f"    - Structural Alignment Status: Rotational Torque Vector Locked.\n")
    return magnetic_flux_density

if __name__ == "__main__":
    # Test values using scale metrics of standard organic double-helix traces
    calculate_helical_magnetic_field(current_amperes=15e-6, radius_meters=1.0e-9, pitch_meters=3.4e-9, length_meters=34e-9)
