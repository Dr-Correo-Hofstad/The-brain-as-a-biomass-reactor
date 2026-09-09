// ====================================================================
// DESIGN STANDARDS: RT Solid-State Thermal Management Block
// Phononic Crystal Heat Sink & Mechanical Vibration Isolator
// ====================================================================

$fn = 60;

sink_base_x = 80.0;
sink_base_y = 80.0;
sink_base_z = 6.0;
fin_count_x = 8;
fin_count_y = 8;

module solid_state_phonon_sink() {
    // 1. Primary Thermal Intake Substrate
    color("DarkCopper", 1.0)
        cube([sink_base_x, sink_base_y, sink_base_z], center = true);
        
    // 2. Parametric Array of Phononic Crystal Scatterer Fins
    // These structures scatter thermal phonon paths to optimize dissipation area
    translate([-(sink_base_x/2), -(sink_base_y/2), sink_base_z/2]) {
        for (x = [1 : fin_count_x]) {
            for (y = [1 : fin_count_y]) {
                spacing_x = sink_base_x / (fin_count_x + 1);
                spacing_y = sink_base_y / (fin_count_y + 1);
                
                translate([x * spacing_x, y * spacing_y, 12.0]) {
                    difference() {
                        // Conical dissipator topology
                        cylinder(h = 24.0, r1 = 3.5, r2 = 1.2, center = true);
                        // Internal acoustic reflection slot
                        cube([1.0, 7.0, 26.0], center = true);
                    }
                }
            }
        }
    }
}

module core_grounding_flange() {
    // Spatial positioning brackets to anchor the sink to the chassis chassis wall
    for (pos_x = [-42, 42]) {
        translate([pos_x, 0, -3.0]) {
            difference() {
                cube([10.0, 20.0, 4.0], center = true);
                cylinder(h = 8.0, r = 2.5, center = true);
            }
        }
    }
}

// System Matrix Assembly
union() {
    solid_state_phonon_sink();
    color("Zinc", 0.5) core_grounding_flange();
}
