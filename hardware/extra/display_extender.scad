include <../headset/display_frame.scad>;

extend_length = 10;
base_width = 55;

union() {
    linear_extrude(dframe_base_thick)
    translate([-dframe_base_w/2, -dframe_base_h/2])
    difference() {
        square([dframe_base_w, dframe_base_h]);
        dframe_reference_nut_holes();
    }

    translate([-base_width/2, -base_width/2])
    cube([base_width, base_width, extend_length]);

    translate([0,0,extend_length - dframe_base_thick])
    union() {
        translate([0,0,dframe_base_thick/2])
        cube([dframe_base_w - 2*dframe_side_thick, dframe_base_h, dframe_base_thick], center=true);
        dframe_full();
    }
}

