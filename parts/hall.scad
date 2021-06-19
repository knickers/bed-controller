// Space between moving parts
tolerance = 0.2; // [0.05:0.05:0.5]

// Wall Thickness
wall = 2; // [1:0.5:5]

// Steel Thickness
steel = 5; // [2:0.5:10]

// Rivet Diameter
rivet = 19.5; // [15:0.5:25]

// Radius to Support Block
radius = 20; // [10:1:30]

// Magnet Diameter
magnet_d = 8; // [1:1:20]

// Magnet Height
magnet_h = 3; // [1:0.5:5]

// Angle Between Magnets
magnet_a = 65; // [20:1:90]

// Distance Between Magnets
distance = 15; // [10:1:20]

// Radius to center of magnet arc
magnet_r = distance/2 / sin(magnet_a/2);

// Resolution
$fs = 2; // [1:High, 2:Medium, 4:Low]
$fa = 0.01 + 0;

part = "Combined"; // [Combined, Separated, Rotor, Stator Front, Stator Back]

if (part == "Combined") {
	difference() {
		union() {
			translate([0, 0, magnet_h+wall*2+tolerance])
				rotate(180, [1,0,0])
					color("red") stator_back();

			rotate(magnet_a/2, [0,0,1])
				translate([0, 0, -wall*2-tolerance])
					color("blue") stator_front();

			translate([0, 0, 0])
				rotor();
		}
		translate([0, 0, -rivet/2])
			cube(rivet);
	}
}
else if (part == "Separated") {
	translate([-rivet, 0, 0])
		rotor();
	translate([rivet, 0, 0])
		stator_front();
	translate([0, rivet, 0])
		stator_back();
}
else if (part == "Rotor") {
	rotor();
}
else if (part == "Stator Front") {
	stator_front();
}
else if (part == "Stator Back") {
	stator_back();
}

module stator_front() {
	flange = rivet+wall*0+tolerance*2;
	difference() {
		union() {
			cylinder(d=rivet-wall*4, h=wall*3); // Insert
			cylinder(d=flange, h=wall*2);       // Flange
			linear_extrude(wall)
				polygon([                       // Sensor tab
					[-magnet_r-2, 2],    // Q2
					[-magnet_r-2, -2],   // Q3
					[-rivet/2+wall, -wall*3], // Q4
					[-rivet/2+wall, wall*3]   // Q1
				]);
		}
		translate([0, 0, -1])
			cylinder(d=rivet-wall*6, h=wall*3+2, $fn=7);// Center Hole
		translate([-flange/2.025-1, -1.5, -1])
			cube([1, 3, wall*2+2]);               // Vertical Wire Cutout
		translate([-rivet, -1.5, wall])
			cube([rivet, 3, 1]);               // Horizontal Wire Cutout
	}
}

module stator_back() {
	h = magnet_h + wall*2 + tolerance*2;
	flange = rivet + wall*2 + tolerance*2;

	difference() {
		union() {
			cylinder(d=rivet-wall*2, h=h);                // OD
			cylinder(d=flange, h=wall*2);                 // Flange
		}

		translate([0, 0, wall*2])
			cylinder(d=rivet-wall*4, h=wall*2);           // ID

		translate([0, 0, -1])
			cylinder(d=rivet, h=wall+1);                  // Rivet cutout

		translate([-flange/2, rivet/2, 0])
			rotate(-45, [1,0,0])
				cube([flange, wall*2, h]);                // Steel corner cutout

		//translate([0, 0, -1]) cube(rivet);
	}
}

module rotor() {
	or = rivet/2 + tolerance; // Outside Radius
	mag = magnet_d / 2;

	difference() {
		union() {
			translate([-radius-mag-wall, 0, 0])
				cylinder(r=mag+wall, h=magnet_h+steel+wall*2+tolerance);
					// Support block

			linear_extrude(magnet_h)
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
			cylinder(r=or-wall, h=magnet_h+2);         // ID

		rotate(-magnet_a/2, [0,0,1])
			translate([-magnet_r, 0, -1])
				cylinder(d=magnet_d+tolerance, h=magnet_h+tolerance+2); // Upper

		rotate(magnet_a/2, [0,0,1])
			translate([-magnet_r, 0, -1])
				cylinder(d=magnet_d+tolerance, h=magnet_h+tolerance+2); // Lower
	}
}
