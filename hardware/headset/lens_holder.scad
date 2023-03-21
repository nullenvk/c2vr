include <params.scad>;

lens_holder_outer = 6.5;
lens_holder_inner = 2;
lens_holder_holes_n = 3;
lens_holder_angle = 30;
lens_holder_h = 1;

module lens_holder_holes() {
    hole_r = M2_5_r;

    module hole() {
        translate([lens_d/2 + lens_holder_outer/2,0,0]) circle(r=hole_r);
    }

    for(i = [0:lens_holder_holes_n-1])
        rotate([0, 0, i * 360/lens_holder_holes_n + lens_holder_angle]) hole();
}

module lens_holder() {
    linear_extrude(lens_holder_h)
    difference() {
        circle(r=lens_d/2 + lens_holder_outer);
        circle(r=lens_d/2 - lens_holder_inner);

        lens_holder_holes();
    }
}
