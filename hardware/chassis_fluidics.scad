// ====================================================================
// DESIGN STANDARDS: RT Microfluidic Infrastructure Block
// Modeling Sinuous Exhaust Manifold and High-Viscosity Routing Channels
// ====================================================================

$fn = 100; // Smooth curves for minimized boundary layer fluidic drag

// Parametric Dimensional Definitions (All metrics in millimeters)
manifold_x = 160.0;
manifold_y = 120.0;
manifold_z = 35.0;
wall_thickness = 4.0;
channel_radius = 2.5;

module microfluidic_substrate() {
    difference() {
        // Base Solid Manifold Block
        color("Teal", 0.6)
            cube([manifold_x, manifold_y, manifold_z], center = true);
        
        // --- Sinuous Exhaust Channel Array ---
        // S-Curve Routing 1
        for (i = [-3 : 3]) {
            translate([i * 18, 0, 5])
                rotate([90, 0, 0])
                    cylinder(h = 90, r = channel_radius, center = true);
        }
        
        // Sinuous Cross-Connect Headers
        translate([0, 45, 5])
            rotate([0, 90, 0])
                cylinder(h = 110, r = channel_radius, center = true);
                
        translate([0, -45, 5])
            rotate([0, 90, 0])
                cylinder(h = 110, r = channel_radius, center = true);
        
        // --- High-Viscosity Fluidic Injection Mainliner ---
        translate([0, 0, -8])
            rotate([0, 90, 0])
                cylinder(h = manifold_x + 2, r = channel_radius * 1.5, center = true);
                
        // Vertical Delivery Vias (Connecting Feed Lines to Logic Array Plenums)
        for (offset_x = [-45, 0, 45]) {
            translate([offset_x, 0, -10])
                cylinder(h = 20, r = 1.8, center = true);
        }
        
        // Pneumatic Pressure Relief Exhaust Port
        translate([manifold_x / 2 - 12, manifold_y / 2 - 12, 0])
            cylinder(h = manifold_z + 2, r = 6.0, center = true);
    }
}

module interface_seal_gaskets() {
    // High-precision seal collars for input/output plumbing couplers
    z_pos = -(manifold_z / 2) + 1.5;
    for (offset_x = [-45, 0, 45]) {
        translate([offset_x, 0, z_pos]) {
            difference() {
                cylinder(h = 3.0, r = 4.5, center = true);
                cylinder(h = 4.0, r = 1.8, center = true);
            }
        }
    }
}

// System Compilation Assembly
union() {
    microfluidic_substrate();
    color("Crimson", 1.0) interface_seal_gaskets();
}
