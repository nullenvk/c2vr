include <params.scad>;
include <display_frame.scad>;
include <lens_holder.scad>;

DEBUG_SCREEN_HOLE = true;

modules_space = [0, 
            lens_focal + screen_d - lens_thick/2,
            12 + M2_5_nut_l + panel_d,
            20,
];

// Cumulative sum of module sizes
modules_pos = [ for (a=0, b=modules_space[0]; a < len(modules_space); a= a+1, b=b+modules_space[ min(a, len(modules_space) - 1) ]) b];
case_length = modules_pos[len(modules_pos) - 1] + (len(modules_pos) - 2) * panel_d;

displays_y_offset = (panel_h - dframe_base_h - TE_ERR)/2;
case_inner_w = panel_w + 2 * TE_ERR;
case_inner_h = panel_h + 2 * TE_ERR; 
case_outer_w = case_inner_w + 2 * case_thickness;
case_outer_h = case_inner_h + 2 * case_thickness;


// Variables validation section
assert(panel_w > lens_d*2);

module lens_approx() {
    rotate([90,0,0]) {
        union() {
            translate([0,0,-EPSILON])
            cylinder(lens_thick + EPSILON, d=lens_d + lens_outer + TE_ERR/2);
            
            translate([0,0,lens_thick - EPSILON])
            cylinder(panel_d - lens_thick + EPSILON*2, d=lens_d);
        }
    }
}

module ipd_mirror() {
    translate([-IPD/2,0,0]) children();
    translate([IPD/2,0,0]) children();
}

module lens_sockets() {
    ipd_mirror() lens_approx();
}

module panel_bare(w, h) {
    cr = case_roundness;

    hull() {
        square([w - cr, h - cr], center=true);
        for(i = [[-w/2 + cr, -h/2 + cr], [w/2 - cr,-h/2 + cr], [-w/2 + cr, h/2 - cr], [w/2 - cr, h/2 - cr]])
        translate(i) circle(r=case_roundness);
    }
}

module panel_base(w, h) {
    difference() {
        panel_bare(w, h);

        holder_hole();
        mirror([1,0,0]) holder_hole();
    }
}

module panel() {
    rotate([90,0,0])
    linear_extrude(panel_d)
    panel_base(panel_w, panel_h);
}

module nose_shape() {
    translate([0,0,-panel_h/2 + displays_y_offset]) {
        hull() {
            translate([0,0,-5])
            scale([0.85,1,2.1])
            sphere(r=19.25);

            translate([0,20,-20])
            scale([1.0,2,1])
            sphere(r=25);
        }
    }

}

module panel_lens_nose() {
    intersection() {
        difference() {
            scale([1.1, 1.1, 1.1]) nose_shape();
            nose_shape();
        }

        let(w = panel_w, h = panel_h) {
            translate([-w/2, -modules_space[1] - panel_d, -h/2])
            cube([w, modules_space[1], h]);
        }
    }
}

module panel_lens() {
    difference() {
        rotate([90,0,0])
        linear_extrude(panel_d) {
            difference() {
                panel_base(panel_w, panel_h);

                translate([0, displays_y_offset]) ipd_mirror() lens_holder_holes();
            }
        }

        translate([0, 0, displays_y_offset]) lens_sockets();
        nose_shape();
    }
}

module panel_display() {
    hole_h = panel_h - dframe_base_h - TE_ERR;
    hole_w = panel_w - case_roundness*2;

    y_offset = displays_y_offset; 

    module pd_base() {
        rotate([90,0,0])
        linear_extrude(panel_d)
        difference() {
            panel_base(panel_w, panel_h);
            translate([0, hole_h/2 - panel_h/2, 0])
                square([hole_w, hole_h], center=true);

            translate([0, y_offset])
            ipd_mirror() dframe_mount_pattern();
        }
    }
    
    difference() {
        pd_base();
        translate([0, 0, y_offset]) display_reference_both(IPD);
    }
}

module mirror_cross() {
    children();
    mirror([1,0,0]) children();

    mirror([0,1,0]) {
        children();
        mirror([1,0,0]) children();
    }
}

module display_controller_holes() {
    module dhole()
        translate([disp_con_w/2 - 4, disp_con_h/2 - 4]) circle(r=M2_5_r + 0.1); // M2.5

    mirror_cross() dhole();
}

module mpu_holes() {
    module hole()
        translate([mpu_w/2 - M2_5_r, mpu_h/2 - M2_5_r]) circle(r=M2_5_r + 0.1); // M2.5

    mirror_cross() hole();
}

module panel_controller() {
    //hole_h = (panel_h - dframe_base_h)/2;
    hole_h = controller_panel_hole_h;
    hole_w = panel_w - case_roundness*2;

    rotate([90,0,0])
    linear_extrude(panel_d)
    difference() {
        panel_base(panel_w, panel_h);

        translate([0,hole_h/2 - panel_h/2, 0])
            square([hole_w, hole_h], center=true);

        translate([0,disp_con_h/2 - 2.5,0]) display_controller_holes();
        translate([-mpu_w,-mpu_h - 5,0]) mpu_holes();
    }
}

function add(v, i = 0, r = 0) = i < len(v) ? add(v, i + 1, r + v[i]) : r;

module case_base() {
    difference() {
        panel_bare(case_outer_w, case_outer_h);
        panel_bare(case_inner_w, case_inner_h + 0.3);
    }
}

module holder_hole() {
    let(w = panel_w + 2 * case_thickness, h = panel_h + 2 * case_thickness) {
        translate([w/2 - holder_w/2 - (M2_5_r + 0.1), - module_hole_dist])
        circle(r=M2_5_r + 0.1); 
    }
}

module module_holder(single=false) {
    module holder_single() {
        translate([0, -case_gap_outer, 0])
        rotate([90,0,0])
        linear_extrude(holder_d)
        difference() {
            panel_base(panel_w +  case_thickness, panel_h + case_thickness);

            let(w = panel_w + 2 * case_thickness, h = panel_h + 2 * case_thickness) {
                translate([0,h/4])
                square([w,h/2], center=true);
            
                square([w - 2 * holder_w, h], center = true);
            
                holder_hole();
                mirror([1,0,0]) holder_hole();
            }
        }
    }

    color([0,1,0])
    union() {
        translate([0,holder_d,0]) holder_single();
        if(!single)
            translate([0,-holder_space, 0]) holder_single();
    }
}

module case_side_mount() {
    a = 15;
    r = 0.9 * a;
    l = case_length; 

    hole_n = 2;
    hole_a = 6.01 + 0.1;
    hole_d = 2;

    module mount_hole() { // M2.5
        hole_r = M2_5_r + 0.1; 

        let(box_x = (hole_r - hole_a)/2) {
            translate([a-r + r/2, 0, 0]) {
                    translate([box_x, hole_d, box_x]) cube([hole_a, a, hole_a]);
                    translate([hole_r/2, 0, hole_r/2]) rotate([-90,0,0]) cylinder(r=hole_r, h = a);
            }
        }
    }

    translate([-a+r,0,0])
    scale([1,1,1.75])
    mirror([0,1,1]) 
    difference() {
        linear_extrude(l, convexity=10)
        difference() {
            square(a);
            translate([a,a,0]) circle(r=r);
        }
        
        for (i = [1 : hole_n]) {
            hole_spacing = case_length / (hole_n+1);
            translate([0, 0, hole_spacing * i]) mount_hole();
        }
    } 
}

module case_bottom() {
    module case_bottom_bare() {
        rotate([90,0,0])
        linear_extrude(case_length, convexity=10)
        difference() {
            case_base();

            translate([0, case_outer_h / 4])
            square([case_outer_w, case_outer_h / 2], center=true);
        }
    }

    union() {
        difference() {
            case_bottom_bare();
            translate([0, -case_gap_outer - modules_pos[0], 0])
            nose_shape();
        }

        // Make the last holder one-sided
        for(i = [0 : len(modules_pos) - 2]) translate([0,-modules_pos[i],0]) module_holder();
        translate([0, -modules_pos[len(modules_pos) - 1], 0]) module_holder(single=true);

        translate([case_outer_w / 2,0,0])
        case_side_mount();
        
        mirror([1,0,0]) translate([case_outer_w / 2,0,0])
        case_side_mount();
    }
}

module strap_mount() {
    sm_thick = 5;
    sm_gw = 20 + 5;
    sm_gh = 5;
    
    sm_h = sm_gh + sm_thick;
    sm_w = sm_gw + 2 * sm_thick;

    sm_d = 5;

    translate([0,-sm_w/2,0])
    rotate([0,0,90])
    linear_extrude(sm_d)
    difference() {
        square([sm_w, sm_h]);
        translate([(sm_w - sm_gw)/2, 0]) square([sm_gw, sm_gh]);
    }
}

module strap_mount_r() {
    translate([-panel_w/2 - 2 * case_thickness, 0, strap_mount_zside])
    strap_mount();
}

module face_side() {
    cutout_r = 64;
    cutout_s = [1.7, 5, 1.2];
    cutout_p = 0;

    entire_scale = 2;
    facing_scale = 2.5;
    facing_len = 3;

    module face_side_base(r_outer, r_inner) {
        difference() {
            linear_extrude(face_len, convexity=15)
            difference() {
                panel_bare(panel_w + r_outer, panel_h + r_outer);
                panel_bare(panel_w - r_inner, panel_h - r_inner);
            }
            
            translate([0,0,-face_len - cutout_p])
            scale(cutout_s)
            sphere(r=cutout_r);

        }
    }

    difference() {
        union() {
            face_side_base(2 * entire_scale * case_thickness, 0);
            let(s = facing_scale * entire_scale * case_thickness) 
            difference() {
                face_side_base(s, s);
                translate([0,0,facing_len]) face_side_base(s + EPSILON, s + EPSILON);
                
            }
        }

        rotate([-90,0,0])
        translate([0,-face_len - case_gap_outer - modules_pos[0], 0])
        nose_shape();
    }
}

module face_panel() {
    // Face panel
    translate([0,face_len,0])
    rotate([90,0,0])
    union() {
        face_side();

        // Strap mounts
        translate([0, 0, face_len])
        mirror([0,0,1]) {
            mirror([1,0,0]) strap_mount_r();
            strap_mount_r();
        }
            
        translate([0, panel_h/2 + 1*case_thickness, face_len + strap_mount_ztop])
        rotate([0,0,-90])
        strap_mount();

    }
}

module case_top() {
    module display_board_hole() {
        dph_h = 9;

        translate([-disp_con_w/2, 
                -modules_pos[2] - panel_d - dph_h - case_gap_outer, 
                panel_h/2])
        cube([disp_con_w, dph_h, case_thickness*2]);
    }

    union() {
        face_panel();

        // Actual top case
        difference() {
            union() {
                rotate([90,0,0])
                linear_extrude(case_length, convexity=20) 
                difference() {
                    case_base();

                    translate([0,-case_outer_h / 4])
                    square([case_outer_w, case_outer_h / 2], center=true);
                }


                mirror([0,0,1]) {
                    translate([case_outer_w / 2, 0, 0])
                    case_side_mount();

                    mirror([1,0,0]) translate([case_outer_w / 2, 0, 0])
                    case_side_mount();
                }
            }

            display_board_hole();
        }
    }
}

module debug_screen_hole() {
    extra_gap = panel_d * 1.8;

    hole_w = panel_w - holder_w * 2;
    hole_h = case_thickness * 2;
    hole_d = modules_space[2] - panel_d + extra_gap * 2;

    hole_z = modules_pos[2] + case_gap_outer + extra_gap;

    translate([-hole_w/2, -hole_z, -hole_h - panel_h/2])
    cube([hole_w, hole_d, hole_h]);
}

// Demo showcase:
union() {
    difference() {
        case_bottom();
        if(DEBUG_SCREEN_HOLE) debug_screen_hole();
    }

    //translate([0,0,panel_h * 0]) color([0.2,0.5,1]) case_top();
    
    translate([0,-case_gap_outer - modules_pos[0], 0]) panel_lens();
    color([0,1,0]) translate([0,-case_gap_outer - modules_pos[0], 0]) panel_lens_nose();

    translate([0,-case_gap_outer - modules_pos[1], 0]) panel_display();
    translate([0,-case_gap_outer - modules_pos[2], 0]) panel_controller();
    translate([0,-case_gap_outer - modules_pos[3], 0]) panel();
}
