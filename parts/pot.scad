// Tolerance between moving parts
T  = 0.2;   // [0.1:0.05:0.5]
T2 = T / 2; // Half of the part tolerance

S  = 4;     // Steel thickness
P  = 2 + 0; // Perimeter wall thickness


/***************************************
 *  Layout all the parts for printing  *
 ***************************************
 */

/* */
head_dynamic();
/* */

/* * /
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

	// Clip
	difference() {
		cube([x, y, S+2*P]);
		translate([P, P, P])
			cube([x, y, S]);
	}

	// Pot shaft plate
	translate([0, P-d, -7]) {
		difference() {
			translate([0, -5, -5])
				cube([P, 3+d+y, 12+P]); // Main plate

			translate([-1, 0, 0])
				rotate(90, [0,1,0])
					#cylinder(d=6, h=P+2); // Center hole
		}
	}
}

module head_static() {
	x = 10;
	y = 20;
	d = 15;

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
					cylinder(d=8, h=P+2); // Center hole

			translate([-1 , -1-12, -1.5])
				cube([P+2, 2+1, 3]); // Key notch
		}
	}
}
