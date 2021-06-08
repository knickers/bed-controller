// Wall Thickness
wall = 2; // [1:0.5:5]
// Steel Thickness
steel = 3; // [2:0.5:4]
// Rivet Diameter
rivet = 20; // [15:0.5:25]
// Pivot Radius
radius = 16; // [10:0.5:20]
// Part Height
height = 5; // [1:1:20]
tolerance = 0.2; // [0.05:0.05:0.5]
// Magnet Thickness
mag_thick = 3; // [1:0.5:5]
// Magnet Diameter
magnet = 8; // [1:1:20]
// Distance Between Magnets
distance = 16; // [10:1:20]

// Resolution
$fs = 2; // [1:High, 2:Medium, 4:Low]
$fa = 0.01 + 0;

part = "comb"; // [comb:Combined, sep:Separated, static:Stationary, dyn:Rotator]

if (part == "comb") {
	translate([0, 0, -wall*2-tolerance])
		rotate(22, [0,0,1])
			color("red")
				stationary_pivot();

	translate([0, 0, 0])
		rotating_pivot();
}
else if (part == "sep") {
	translate([rivet*2, 0, 0])
		stationary_pivot();
}

module stationary_pivot() {
	difference() {
		union() {
			cylinder(d=rivet+wall*2, h=height+wall*2+tolerance*2); // OD
			cylinder(d=rivet+wall*4+tolerance*2, h=wall*2);        // Flange
			/*
			translate([0, 0, height+wall+tolerance*2])
				cylinder(d=rivet+wall*4+tolerance*2, h=wall);    // Upper Flange
			*/
			linear_extrude(wall)                                   // Sensor tab
				polygon([
					[-radius-magnet, 1], // Q2
					[-radius-magnet, -1], // Q3
					[-rivet/2-wall, -wall*3], // Q4
					[-rivet/2-wall, wall*3] // Q1
				]);
		}
		translate([0, 0, -1])
			cylinder(d=rivet, h=height+wall*2+tolerance+2);      // ID
		translate([-radius, -1.5, -1])
			cube([1, 3, wall+2]);                                // Wire Cutout
	}
}

module rotating_pivot() {
	od = rivet+wall*4+tolerance*2;

	difference() {
		cylinder(d=od, h=height);               // OD
		translate([0, 0, -1])
			cylinder(d=od-wall*2, h=height+2);  // ID
	}

	r = radius + magnet*0.6;
	a = (distance+magnet) / od * 50 - 40.7;

	rotate(-a, [0,0,1])
		translate([-r, rivet/2+wall+tolerance, 0])
			cube([r, wall, height]);                   // Upper arm
	rotate(a, [0,0,1])
		translate([-r, -rivet/2-wall*2-tolerance, 0])
			cube([r, wall, height]);                          // Lower arm

	magnet_holder();
}

module magnet_holder() {
	H = height + steel + wall + tolerance;
	R = radius;         // Inner radius of arc
	D = magnet+wall*2;  // Distance between IR and OR
	r = R+D/2;          // Radius to center of arc
	a = asin(distance/r) / 2;  // Angle between magnets
	A = sin($t*360) * a;// Animation angle offset
	difference() {
		rotate(180-a + A, [0,0,1])
			union() {
				rotate_extrude(angle=a*2)
					translate([R, 0, 0]) {
						square([D, H]);
					}
				translate([r, 0, 0])
					cylinder(d=D, h=H);
				rotate(a*2, [0,0,1])
					translate([r, 0, 0])
						cylinder(d=D, h=H);
			}

		rotate(-a + A, [0,0,1])
			translate([-r, 0, -1])
				cylinder(d=magnet+tolerance, h=mag_thick+1);   // Upper magent

		rotate(a + A, [0,0,1])
			translate([-r, 0, -1])
				cylinder(d=magnet+tolerance, h=mag_thick+1);   // Lower magent
	}
}
