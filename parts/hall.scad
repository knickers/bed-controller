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

// Resolution
$fs = 2; // [1:High, 2:Medium, 4:Low]
$fa = 0.01 + 0;

part = "comb"; // [comb:Combined, sep:Separated, static:Stationary, dyn:Rotator]

if (part == "comb") {
	translate([0, 0, height+wall+tolerance])
		rotate(180, [1,0,0])
			color("red") stationary_back();

	rotate(22, [0,0,1])
		translate([0, 0, -wall*2-tolerance])
			color("yellow") stationary_front();

	translate([0, 0, 0])
		rotating_pivot();
}
else if (part == "sep") {
	translate([rivet*2, 0, 0])
		stationary_pivot();
}

module stationary_front() {
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

module stationary_back() {
	difference() {
		union() {
			cylinder(d=rivet+wall*2, h=height+wall+tolerance*2); // OD
			cylinder(d=rivet+wall*4+tolerance*2, h=wall);        // Flange
		}
		translate([0, 0, -1])
			cylinder(d=rivet, h=height+wall+tolerance+2);        // ID
	}
}

module rotating_pivot() {
	or = rivet/2 + wall*2 + tolerance;

	difference() {
		cylinder(r=or, h=height);             // OD
		translate([0, 0, -1])
			cylinder(r=or-wall, h=height+2);  // ID
	}

	// soh cah toa

	l = cos(magnet_a) * magnet_r;        // Horizontal Length at Support Arm
	h = (distance+magnet)/2 + wall - or; // Vertical height at support arm
	a = atan(h / l);

	rotate(-a, [0,0,1])
		translate([-l, or-wall, 0])
			#cube([l, wall, height]);                   // Upper arm

	rotate(a, [0,0,1])
		translate([-l, -or, 0])
			#cube([l, wall, height]);                    // Lower arm

	magnet_holder();
}

module magnet_holder() {
	H = height + steel + wall + tolerance;
	D = magnet+wall*2;          // Distance between IR and OR
	A = sin($t*360) * magnet_a; // Animation angle offset
	difference() {
		rotate(180-magnet_a + A, [0,0,1])
			union() {
				rotate_extrude(angle=magnet_a*2)
					translate([radius, 0, 0])
						square([D, H]);         // Main body arc
				translate([magnet_r, 0, 0])
					cylinder(d=D, h=H);         // Lower Rounded Edge
				rotate(magnet_a*2, [0,0,1])
					translate([magnet_r, 0, 0])
						cylinder(d=D, h=H);     // Upper Rounded Edge
			}

		rotate(-magnet_a + A, [0,0,1])
			translate([-magnet_r, 0, -1])
				cylinder(d=magnet+tolerance, h=mag_thick+1);   // Upper magent

		rotate(magnet_a + A, [0,0,1])
			translate([-magnet_r, 0, -1])
				cylinder(d=magnet+tolerance, h=mag_thick+1);   // Lower magent
	}
}
