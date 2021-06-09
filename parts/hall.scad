// Space between moving parts
tolerance = 0.2; // [0.05:0.05:0.5]

// Wall Thickness
wall = 2; // [1:0.5:5]

// Steel Thickness
steel = 5; // [2:0.5:10]

// Rivet Diameter
rivet = 20; // [15:0.5:25]

// Radius to Support Block
radius = 18; // [10:1:30]

// Part Height
height = 4; // [1:1:20]

// Magnet Thickness
magnet_t = 3; // [1:0.5:5]

// Magnet Diameter
magnet_d = 8; // [1:1:20]

// Angle Between Magnets
magnet_a = 60; // [20:1:90]

// Distance Between Magnets
distance = 16; // [10:1:20]

// Radius to center of magnet arc
magnet_r = distance/2 / sin(magnet_a/2);

// Resolution
$fs = 2; // [1:High, 2:Medium, 4:Low]
$fa = 0.01 + 0;

part = "Combined"; // [Combined, Separated, Stator, Rotor]

if (part == "Combined") {
	translate([0, 0, height+wall*2+tolerance])
		rotate(180, [1,0,0])
			color("red") stator_back();

	rotate(magnet_a/2, [0,0,1])
		translate([0, 0, -wall*2-tolerance])
			color("yellow") stator_front();

	translate([0, 0, 0])
		rotor();
}
else if (part == "Separated") {
	translate([-rivet, 0, 0])
		rotor();
	translate([rivet, 0, 0])
		stator_front();
	translate([0, rivet, 0])
		stator_back();
}
else if (part == "Stator") {
	translate([0, -rivet, 0])
		stator_front();
	translate([0, rivet, 0])
		stator_back();
}
else if (part == "Rotor") {
	rotor();
}

module stator_front() {
	flange = rivet+wall*2+tolerance*2;
	difference() {
		union() {
			cylinder(d=rivet-wall*2, h=wall*3); // Insert
			cylinder(d=flange, h=wall*2);       // Flange
			linear_extrude(wall)
				polygon([                       // Sensor tab
					[-magnet_r-2, 1],    // Q2
					[-magnet_r-2, -1],   // Q3
					[-rivet/2, -wall*3], // Q4
					[-rivet/2, wall*3]   // Q1
				]);
		}
		translate([-flange/2-1, -1.5, -1])
			cube([1, 3, wall+2]);               // Wire Cutout
	}
}

module stator_back() {
	difference() {
		union() {
			cylinder(d=rivet, h=height+wall*2+tolerance*2);        // OD
			cylinder(d=rivet+wall*2+tolerance*2, h=wall*2);        // Flange
		}

		translate([0, 0, -1])
			cylinder(d=rivet-wall*2, h=height+wall*2+tolerance+2); // ID

		translate([0, 0, wall])
			cylinder(r1=rivet/2, r2=rivet/2-wall, h=wall);         // Cone

		cylinder(r=rivet/2, h=wall);                               // Cone shaft
	}
}

module rotor() {
	or = rivet/2 + wall + tolerance; // Outside Radius
	mag = magnet_d / 2;

	difference() {
		union() {
			translate([-radius-mag-wall, 0, 0])
				cylinder(r=mag+wall, h=height+steel+wall*2+tolerance);
					// Support block

			linear_extrude(height)
				hull() {
					circle(or);

					rotate(-magnet_a/2, [0,0,1])
						translate([-magnet_r, 0, 0])
							circle(mag + wall);        // Upper magnet

					rotate(magnet_a/2, [0,0,1])
						translate([-magnet_r, 0, 0])
							circle(mag + wall);        // Lower magnet

					translate([-radius-mag-wall, 0, 0])
						circle(mag + wall);            // Support Block
				}
		}

		translate([0, 0, -1])
			cylinder(r=or-wall, h=height+2);           // ID

		rotate(-magnet_a/2, [0,0,1])
			translate([-magnet_r, 0, -1])
				cylinder(d=magnet_d+tolerance, h=magnet_t+tolerance+1); // Upper

		rotate(magnet_a/2, [0,0,1])
			translate([-magnet_r, 0, -1])
				cylinder(d=magnet_d+tolerance, h=magnet_t+tolerance+1); // Lower
	}
}
