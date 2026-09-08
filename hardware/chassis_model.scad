// ====================================================================
// DESIGN STANDARDS: RT Physical Infrastructure Framework
// Enclosure Model for Hexadecimal Computing Matrix Core
// ====================================================================

$fn = 120; // Enforce smooth curves for active airflow surfaces

// Design Variables (All dimensions scaled in millimeters)
chassis_length = 240.0;
chassis_width  = 180.0;
chassis_height = 65.0;
wall_thickness = 5.0;
pcb_clearance   = 3.0;

module main_enclosure_frame() {
    difference() {
        // 1. External Protective Shield Structure
        cube([chassis_length, chassis_width, chassis_height], center = true);
        
        // 2. Interior Cavity for the 8-Layer KiCad Board Array
        translate([0, 0, wall_thickness / 2])
            cube([chassis_length - (wall_thickness * 2), 
                  chassis_width - (wall_thickness * 2), 
                  chassis_height - wall_thickness], center = true);
                  
        // 3. Thermal Exhaust Channels (Centrifugal Blower Integration)
        translate([chassis_length / 2 - 25, chassis_width / 2 - 25, 0])
            cylinder(h = chassis_height + 2, r = 22.0, center = true);
            
        translate([-(chassis_length / 2 - 25), -(chassis_width / 2 - 25), 0])
            cylinder(h = chassis_height + 2, r = 22.0, center = true);
            
        // 4. Front Panel Diagnostic Cable Bus (Hex I/O Port Entrance)
        translate([-(chassis_length / 2), 0, -10])
            cube([wall_thickness * 2, 80.0, 15.0], center = true);
    }
}

module circular_gold_mounting_brackets() {
    // Generates 4 robust grounding isolation pins mapped directly to board mounting holes
    offset_x = chassis_length / 2 - 15;
    offset_y = chassis_width / 2 - 15;
    z_pos = -(chassis_height / 2) + pcb_clearance + wall_thickness / 2;
    
    bracket_coordinates = [
        [ offset_x,  offset_y],
        [-offset_x,  offset_y],
        [ offset_x, -offset_y],
        [-offset_x, -offset_y]
    ];
    
    for (pos = bracket_coordinates) {
        translate([pos[0], pos[1], z_pos]) {
            difference() {
                // Outer ring structure
                cylinder(h = 10.0, r = 7.0, center = true);
                // Central mechanical screw shaft path
                cylinder(h = 12.0, r = 3.2, center = true);
            }
        }
    }
}

// ====================================================================
// FINAL SYSTEM RENDERING ASSEMBLY
// ====================================================================
union() {
    color("SlateGray", 0.8) main_enclosure_frame();
    color("Gold", 1.0) circular_gold_mounting_brackets();
}
