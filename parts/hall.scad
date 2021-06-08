// Wall Thickness
wall = 2; // [1:0.5:5]

// Steel Thickness
steel = 3; // [2:0.5:4]

// Rivet Diameter
rivet = 20; // [15:0.5:25]

// Pivot Radius
radius = 18; // [10:0.5:20]

// Part Height
height = 5; // [1:1:20]

tolerance = 0.2; // [0.05:0.05:0.5]

// Magnet Thickness
mag_thick = 3; // [1:0.5:5]

// Magnet Diameter
magnet = 8; // [1:1:20]

// Distance Between Magnets
distance = 16; // [10:1:20]

magnet_r = radius + magnet/2 + wall;  // Radius to center of magnet arc
magnet_a = asin(distance/2/magnet_r); // Angle between magnets
echo("Maximum Angle:", magnet_a*2);

// Resolution
$fs = 2; // [1:High, 2:Medium, 4:Low]
$fa = 0.01 + 0;

part = "Combined"; // [Combined, Separated, Stator, Rotor]

if (part == "Combined") {
	translate([0, 0, height+wall+tolerance])
		rotate(180, [1,0,0])
			color("red") stator_back();

	rotate(magnet_a, [0,0,1])
		translate([0, 0, -wall*2-tolerance])
			color("yellow") stator_front();

	translate([0, 0, 0])
		rotor();
}
else if (part == "Separated") {
	translate([-rivet, 0, 0])
		rotor();
	translate([rivet*2, 0, 0])
		stator_front();
	translate([0, rivet*2, 0])
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
	difference() {
		union() {
			cylinder(d=rivet, h=wall*3);                    // Insert
			cylinder(d=rivet+wall*4+tolerance*2, h=wall*2); // Flange
			linear_extrude(wall)
				polygon([                                   // Sensor tab
					[-radius-magnet, 1],      // Q2
					[-radius-magnet, -1],     // Q3
					[-rivet/2-wall, -wall*3], // Q4
					[-rivet/2-wall, wall*3]   // Q1
				]);
		}
		translate([-radius, -1.5, -1])
			cube([1, 3, wall+2]);                           // Wire Cutout
	}
}

module stator_back() {
	difference() {
		union() {
			cylinder(d=rivet+wall*2, h=height+wall+tolerance*2); // OD
			cylinder(d=rivet+wall*4+tolerance*2, h=wall);        // Flange
		}
		translate([0, 0, -1])
			cylinder(d=rivet, h=height+wall+tolerance+2);        // ID
	}
}

module rotor() {
	or = rivet/2 + wall*2 + tolerance; // Outside Radius
	l = cos(magnet_a) * magnet_r;      // Horizontal Length

	difference() {
		union() {
			translate([-l-magnet/2-wall, -distance/2, 0])
				cube([magnet+wall, distance, height+steel+wall+tolerance]);
			linear_extrude(height)
				hull() {
					circle(or);

					rotate(-magnet_a, [0,0,1])
						translate([-magnet_r, 0, 0])
							circle(magnet/2 + wall);   // Upper rounder

					rotate(magnet_a, [0,0,1])
						translate([-magnet_r, 0, -1])
							circle(magnet/2 + wall);   // Lower rounder
				}
		}

		translate([0, 0, -1])
			cylinder(r=or-wall, h=height+2);           // ID

		rotate(-magnet_a, [0,0,1])
			translate([-magnet_r, 0, -1])
				cylinder(d=magnet+tolerance, h=mag_thick+1);   // Upper magent

		rotate(magnet_a, [0,0,1])
			translate([-magnet_r, 0, -1])
				cylinder(d=magnet+tolerance, h=mag_thick+1);   // Lower magent
	}
}
