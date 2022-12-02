screen_w = 54.24; 
screen_h = 59.02;
screen_d = 1.51;
font_depth = 0.4;

difference() {
    translate([-screen_w/2, -screen_h/2, -screen_d])
        cube([screen_w, screen_h, screen_d]);

    translate([0,0,0.1 - font_depth])
    linear_extrude(font_depth)
    //text("59.24x59.02x1.51mm", halign="center", valign="center", size=4);
    text("SCREEN", halign="center", valign="center", size=6);
}
