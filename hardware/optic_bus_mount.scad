// ====================================================================
// DESIGN STANDARDS: RT Optoelectronic Assembly
// Mounting Substructure and Shield Isolation for Visual Transceiver Array
// ====================================================================

$fn = 80;

// Mechanical Interface Tolerances
sensor_pocket_w = 42.5;
sensor_pocket_h = 42.5;
clearance_depth = 12.0;
shield_wall     = 3.5;

module optoelectronic_shield_chassis() {
    difference() {
        // Main Outer Shield Wall Enclosure (Faraday Attenuation Boundary)
        color("DimGray", 0.9)
            cube([sensor_pocket_w + (shield_wall * 2), 
                  sensor_pocket_h + (shield_wall * 2), 
                  clearance_depth + shield_wall], center = true);
        
        // Internal Pocket Array for Visual Transceiver Board Placement
        translate([0, 0, shield_wall / 2])
            cube([sensor_pocket_w, sensor_pocket_h, clearance_depth + 1], center = true);
            
        // Central Clear Opening for Fiber Optic Cable Bus Junction
        cylinder(h = clearance_depth + shield_wall + 5, r = 8.0, center = true);
        
        // Coaxial Grounding Pin Routing Ports (Left/Right Ports)
        translate([sensor_pocket_w / 2 + 1.5, 0, 0])
            rotate([0, 90, 0])
                cylinder(h = 10, r = 2.0, center = true);
                
        translate([-(sensor_pocket_w / 2 + 1.5), 0, 0])
            rotate([0, 90, 0])
                cylinder(h = 10, r = 2.0, center = true);
    }
}

module isolation_mounting_tabs() {
    // Structural structural ears to secure transceiver housing to mainframe mount
    for (offset_y = [-28, 28]) {
        translate([0, offset_y, -(clearance_depth / 2)]) {
            difference() {
                color("Silver", 1.0)
                    cube([15.0, 10.0, 4.0], center = true);
                // Alignment bolt hole path
                cylinder(h = 6.0, r = 2.2, center = true);
            }
        }
    }
}

// Final Hardware Assembly Merge
union() {
    optoelectronic_shield_chassis();
    isolation_mounting_tabs();
}
