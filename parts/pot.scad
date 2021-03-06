// Tolerance between moving parts
T  = 0.2;   // [0.1:0.05:0.5]
T2 = T / 2; // Half of the part tolerance

S  = 4;     // Steel thickness
P  = 2 + 0; // Perimeter wall thickness

$fn = 16;


/***************************************
 *  Layout all the parts for printing  *
 ***************************************
 */

/* * /
head_dynamic();
/* */

/* */
head_static();
/* */

/************************************
 *  Modules for building each part  *
 ************************************
 */

module head_dynamic() {
	x = 15;
	y = 15;
	d = 30;
	r = 3;

	// Clip
	difference() {
		cube([x, y, S+2*P]);
		translate([P, P, P])
			cube([x, y, S]);
	}

	// Pot shaft plate
	translate([0, P-d, -7]) {
		difference() {
			union() {
				translate([0, r-1, -r-P])
					cube([P, d+y-r-P+1, 12+P]); // Main plate
				rotate(90, [0,1,0])
					cylinder(r=r+P, h=P*2); // Flex clip
			}

			translate([-1, 0, 0])
				rotate(90, [0,1,0]) {
					cylinder(r=r, h=P*2+2); // Center hole

					translate([-12-1, r-1, 0])
						cube([12+1, 1, P*2+2]); // Flex cutout

					/*
					difference() { // Corner rounder
						translate([-10, -10, 0])
							cube([16, 10, P+2]);
						translate([0, 0, -2])
							cylinder(r=r+P, h=P+4);
					}

					translate([-r-r-P-P, -1, 0])
						cube([r+P, d-P+1, P+2]); // Flatten top
					*/
				}
		}
	}
}

module head_static() {
	x = 10;
	y = 20;
	d = 15;
	dowelLength = 16;

	// Clip
	difference() {
		translate([0, -P, -P])
			cube([x, y, S+2*P]);
		translate([-1, 0, 0])
			cube([x+2, y, S]);
	}

	// Extension arm
	translate([0, -d-12, S])
		cube([x, d+12, P]);

	// Pot mounting plate
	translate([x-P, -d, -8.5]) {
		difference() {
			translate([0 , -12, -10])
				cube([P, 25+P, 10+12.5+P]); // Main plate

			translate([-1, 0, 0])
				rotate(90, [0,1,0])
					cylinder(d=7.5, h=P+2); // Center hole

			translate([-1 , -1-12, -1.5])
				cube([P+2, 2+1, 3]); // Key notch
		}
	}
}
