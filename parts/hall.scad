wall = 2; // [1:0.5:5]
steel = 3; // [2:0.5:4]
rivet = 20; // [15:0.5:25]
radius = 16; // [10:0.5:20]
height = 5; // [1:1:20]
tolerance = 0.2; // [0.05:0.05:0.5]
magnet = 8; // [1:1:20]

// Resolution
$fs = 2; // [1:High, 2:Medium, 4:Low]
$fa = 0.01 + 0;

/*
//translate([rivet*2, 0, 0])
	stationary_pivot();
*/

/*
translate([0, 0, 0])
	rotating_pivot();
*/

magnet_holder_new(3);

module stationary_pivot() {
	difference() {
		union() {
			cylinder(d=rivet+wall*2, h=height+wall+tolerance); // OD
			cylinder(d=rivet+wall*4, h=wall);                  // Flange
		}
		translate([0, 0, -1])
			cylinder(d=rivet, h=height+wall+tolerance+2);      // ID
	}
}

module rotating_pivot() {
	difference() {
		cylinder(d=rivet+wall*4+tolerance*2, h=height);        // OD
		translate([0, 0, -1])
			cylinder(d=rivet+wall*2+tolerance*2, h=height+2);  // ID
	}

	translate([-radius, rivet/2+wall+tolerance, 0])
		cube([radius, wall, height]);                          // Upper arm
	translate([-radius, -rivet/2-wall*2-tolerance, 0])
		cube([radius, wall, height]);                          // Lower arm

	magnet_holder(height+steel);
}

module magnet_holder_new(H) {
	l = 15;             // Distance between magnets
	R = radius;         // Inner radius of arc
	D = magnet+wall*2;  // Distance between IR and OR
	r = R+D/2;          // Radius to center of arc
	a = asin(l/r) / 2;  // Angle between magnets
	A = sin($t*360) * a;// Animation angle offset
	difference() {
		rotate(180-a + A, [0,0,1])
			union() {
				rotate_extrude(angle=a*2)
					translate([R, 0, 0])
						square([D, H]);
				translate([r, 0, 0])
					cylinder(d=D, h=H);
				rotate(a*2, [0,0,1])
					translate([r, 0, 0])
						cylinder(d=D, h=H);
			}

		rotate(-a + A, [0,0,1])
			translate([-r, 0, -1])
				cylinder(d=magnet, h=H+2);      // Upper magent
		rotate(a + A, [0,0,1])
			translate([-r, 0, -1])
				cylinder(d=magnet, h=H+2);      // Lower magent
	}
}

module magnet_holder(H) {
	intersection() {
		difference() {
			cylinder(r=radius+wall*2+magnet, h=H);  // OD
			translate([0, 0, -1])
				cylinder(r=radius+wall, h=H+2);     // ID
			rotate(-20, [0,0,1])
				translate([-radius-magnet/2-wall, 0, -1])
					cylinder(d=magnet, h=H+2);      // Upper magent
			rotate(20, [0,0,1])
				translate([-radius-magnet/2-wall, 0, -1])
					cylinder(d=magnet, h=H+2);      // Lower magent
		}
		translate([-rivet*1.5, -rivet/2-wall*2-tolerance, 0])
			cube([rivet, rivet+wall*4+tolerance*2, H]);
	}
}
