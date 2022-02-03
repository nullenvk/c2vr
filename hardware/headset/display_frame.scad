EPSILON = 0.01; // Just a very small number

// In MM
screen_w = 54.24 + 0.4; 
screen_h = 59.02 + 0.4;
screen_d = 1.51;

tapeout_h = 2; // TODO: Measure it correctly

frame_side_thick = 5;
frame_side_h = screen_d;
frame_base_thick = 2;

module display_reference_flat() {
    square([screen_w, screen_h]);
}

module tapeout_reference() {
    square([screen_w, tapeout_h + EPSILON]);
}

module frame_bare() {
    linear_extrude(frame_side_h)
    difference() {
        let(w = 2*frame_side_thick + screen_w,
            h = frame_side_thick + screen_h + tapeout_h)
            square([w,h]);

        translate([frame_side_thick, tapeout_h])
            display_reference_flat();

        translate([frame_side_thick, -EPSILON/2])
            tapeout_reference();
    }
}

module frame_base() {
    linear_extrude(frame_base_thick)
    let(w = 2*frame_side_thick + screen_w,
        h = frame_side_thick + screen_h + tapeout_h)
        square([w,h]);

}

module frame_full() {
    frame_base();
    translate([0,0,frame_base_thick]) frame_bare();
}

frame_full();
