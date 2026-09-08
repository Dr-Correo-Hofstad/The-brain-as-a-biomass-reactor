// OpenSCAD Module for the Hex-Brain Logic Core Enclosure
module hex_brain_chassis() {
    difference() {
        // Main structural enclosure perimeter
        cube([160, 120, 45], center = true);
        
        // Internal cavity for the 8-layer KiCad board assembly
        translate([0, 0, 2])
            cube([154, 114, 40], center = true);
            
        // Exhaust routing channels for the centrifugal thermal management system
        translate([70, 0, 15])
            cylinder(h = 20, r = 15, center = true, $fn = 100);
    }
}

// Instantiate the chassis model
hex_brain_chassis();
