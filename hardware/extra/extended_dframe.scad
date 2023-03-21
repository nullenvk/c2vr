include <../headset/display_frame.scad>;

lens_thick = 2;
extend_length = 10 - lens_thick/2;
base_width = 58;

union() {
    linear_extrude(dframe_base_thick)
    translate([-dframe_base_w/2, -dframe_base_h/2])
    difference() {
        square([dframe_base_w, dframe_base_h]);
        dframe_reference_nut_holes();
    }

    translate([-base_width/2, -dframe_base_h/2])
    cube([base_width, dframe_base_h, extend_length]);

    translate([0,0,extend_length])
    union() {
        translate([0,0,dframe_base_thick/2])
        cube([base_width, dframe_base_h, dframe_base_thick], center=true);

        intersection() {
            cube([base_width, dframe_base_h,500], center=true);
            dframe_full();
        }
    }
}

