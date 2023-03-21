//EPSILON = 0.01; 
//$fn = 50;

include <params.scad>;

// In MM
screen_w = 54.24 + TE_ERR;
screen_h = 59.02 + TE_ERR;
screen_d = 1.51 + TE_ERR;

dframe_screenhole_w = screen_w + TE_ERR;
dframe_screenhole_h = screen_h + TE_ERR;
dframe_screenhole_d = screen_d + TE_ERR;

dframe_tapeout_h = 4; // TODO: Measure it correctly

dframe_side_thick = 2;
dframe_side_h = dframe_screenhole_d;
dframe_base_thick = 1;

dframe_side_w = 4;

holder_edge = 1;
holder_h = 1;

// Calculated
dframe_base_w = 2*dframe_side_thick + screen_w + 2*dframe_side_w;
dframe_base_h = dframe_side_thick + screen_h + dframe_tapeout_h;

dframe_offset_x = -EPSILON;
dframe_offset_y = 2;

dframe_nut_r = 1.25 + TE_ERR * 0.25;
dframe_nut_pos = [
    dframe_base_w/2 - dframe_nut_r - dframe_offset_x,
    dframe_base_h/2 - dframe_nut_r - dframe_offset_y
];

reference_te_err = TE_ERR * 3;

module dframe_dot_pattern(w, h) {
    dot_l = 2.4;
    dot_xn = w/(dot_l*2);
    dot_yn = h/(dot_l*2);

    off_x = dot_l;
    off_y = dot_l;

    for(y=[off_y : dot_l*2 : h - dot_l]) {
        for(x=[off_x : dot_l*2 : w - dot_l])
            translate([x,y]) square([dot_l, dot_l]);
    }
}

module dframe_reference_nut_holes() {
    module hole(mx, my) {
        translate([mx * dframe_nut_pos[0], my * dframe_nut_pos[1]])
            circle(r=dframe_nut_r);
    }
    
    translate([dframe_base_w/2, dframe_base_h/2]) {
        hole(1,1);
        hole(-1,1);
        hole(1,-1);
        hole(-1,-1);
    }
}

module dframe_mount_pattern() {
    translate([-dframe_base_w/2, -dframe_base_h/2]) {
        dframe_reference_nut_holes();
        translate([dframe_side_thick + dframe_side_w, dframe_tapeout_h]) dframe_dot_pattern(screen_w, screen_h);
    }
}

module display_reference_flat() {
    square([dframe_screenhole_w, dframe_screenhole_h]);
}
module display_reference_visible() {
    square([dframe_screenhole_w - 2*holder_edge, dframe_screenhole_h - 1*holder_edge]);
}

module display_reference_both(ipd) {
    rotate([90,0,0])
    linear_extrude(dframe_base_thick, convexity=20)
    square([dframe_base_w + reference_te_err + ipd, dframe_base_h + reference_te_err], center=true);
}


module tapeout_reference() {
    square([dframe_screenhole_w, dframe_tapeout_h + EPSILON]);
}

module dframe_bare() {
    linear_extrude(dframe_side_h)
    difference() {
        square([dframe_base_w - 2*dframe_side_w, dframe_base_h]); // Change later

        translate([dframe_side_thick, dframe_tapeout_h])
            display_reference_flat();

        translate([dframe_side_thick, -EPSILON/2])
            tapeout_reference();
    }
}

module dframe_holder() {
    linear_extrude(holder_h)
    difference() {
        square([dframe_base_w - 2*dframe_side_w, dframe_base_h]); // Change later

        translate([dframe_side_thick + holder_edge, dframe_tapeout_h])
            display_reference_visible();

        translate([dframe_side_thick, -EPSILON/2])
            tapeout_reference();
    }
}

module dframe_base() {
    linear_extrude(dframe_base_thick)
    difference() {
        square([dframe_base_w, dframe_base_h]);

        dframe_reference_nut_holes();
        translate([dframe_side_thick + dframe_side_w, dframe_tapeout_h]) dframe_dot_pattern(screen_w, screen_h);
    }
}

module dframe_full() {
    translate([-dframe_base_w/2, -dframe_base_h/2])
    union() {
        dframe_base();
        translate([dframe_side_w,0,dframe_base_thick]) dframe_bare();
        translate([dframe_side_w,0,dframe_base_thick + dframe_side_h]) dframe_holder();
    }
}
