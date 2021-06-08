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
// Magnet Diameter
magnet = 8; // [1:1:20]
// Distance Between Magnets
distance = 16; // [10:1:20]

// Resolution
$fs = 2; // [1:High, 2:Medium, 4:Low]
$fa = 0.01 + 0;

translate([0, 0, -wall-tolerance])
//translate([rivet*2, 0, 0])
rotate(22, [0,0,1])
	stationary_pivot();
/*
*/

translate([0, 0, 0])
	rotating_pivot();
/*

magnet_holder_new(3);
*/

module stationary_pivot() {
	difference() {
		union() {
			cylinder(d=rivet+wall*2, h=height+wall+tolerance); // OD
			cylinder(d=rivet+wall*4+tolerance*2, h=wall);      // Flange
			translate([-radius-magnet, -wall*2, 0])
				cube([radius, wall*4, wall]);                  // Sensor tab
			translate([-radius-magnet, -wall*2, 0])
				cube([wall, wall*4, height+wall-2]);           // Sensor tower
		}
		translate([0, 0, -1])
			cylinder(d=rivet, h=height+wall+tolerance+2);      // ID
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

	//magnet_holder(height+steel);
	difference() {
		magnet_holder_new(3+height);
		translate([0, 0, -1])
			cylinder(r=radius+magnet+wall/2, h=height+1);
	}
}

module magnet_holder_new(H) {
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
				cylinder(d=magnet+tolerance, h=H+2);      // Upper magent
		rotate(a + A, [0,0,1])
			translate([-r, 0, -1])
				cylinder(d=magnet+tolerance, h=H+2);      // Lower magent
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
