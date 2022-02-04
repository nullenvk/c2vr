EPSILON = 0.01; // Just a very small number
$fn = 50;

// In MM
screen_w = 54.24 + 0.4; 
screen_h = 59.02 + 0.4;
screen_d = 1.51 + 0.4;

tapeout_h = 2; // TODO: Measure it correctly

frame_side_thick = 5;
frame_side_h = screen_d;
frame_base_thick = 2;

side_w = 10;

holder_edge = 2;
holder_h = 1;

// Calculated
frame_base_w = 2*frame_side_thick + screen_w + 2*side_w;
frame_base_h = frame_side_thick + screen_h + tapeout_h;

frame_offset_x = 3;
frame_offset_y = 3;

frame_nut_r = 1.25;
frame_nut_pos = [
    frame_base_w/2 - frame_nut_r - frame_offset_x,
    frame_base_h/2 - frame_nut_r - frame_offset_y
];

module dot_pattern(w, h) {
    dot_r = 1.25;
    //square([w,h]);
    dot_xn = w/(dot_r*2);
    dot_yn = h/(dot_r*2);

    off_x = dot_r;
    off_y = dot_r;

    for(y=[off_y : dot_r*2 : h]) {
        for(x=[off_x : dot_r*2 : w])
            translate([x,y]) square([dot_r, dot_r]);
    }
}

module reference_nut_holes() {
    module hole(mx, my) {
        translate([mx * frame_nut_pos[0], my * frame_nut_pos[1]])
            circle(r=frame_nut_r);
    }
    
    translate([frame_base_w/2, frame_base_h/2]) {
        hole(1,1);
        hole(-1,1);
        hole(1,-1);
        hole(-1,-1);
    }
}

module display_reference_flat() {
    square([screen_w, screen_h]);
}
module display_reference_visible() {
    square([screen_w - 2*holder_edge, screen_h - 2*holder_edge]);
}

module tapeout_reference() {
    square([screen_w, tapeout_h + EPSILON]);
}

module frame_bare() {
    linear_extrude(frame_side_h)
    difference() {
        square([frame_base_w - 2*side_w, frame_base_h]); // Change later

        translate([frame_side_thick, tapeout_h])
            display_reference_flat();

        translate([frame_side_thick, -EPSILON/2])
            tapeout_reference();
    }
}

module frame_holder() {
    linear_extrude(holder_h)
    difference() {
        square([frame_base_w - 2*side_w, frame_base_h]); // Change later

        translate([frame_side_thick + holder_edge, tapeout_h])
            display_reference_visible();

        translate([frame_side_thick, -EPSILON/2])
            tapeout_reference();
    }
}

module frame_base() {
    linear_extrude(frame_base_thick)
    difference() {
        square([frame_base_w, frame_base_h]);

        reference_nut_holes();
        translate([frame_side_thick + side_w, tapeout_h]) dot_pattern(screen_w, screen_h);
    }
}

module frame_full() {
    translate([-frame_base_w/2, -frame_base_h/2])
    union() {
        frame_base();
        translate([side_w,0,frame_base_thick]) frame_bare();
        translate([side_w,0,frame_base_thick + frame_side_h]) frame_holder();
    }
}

frame_full();
