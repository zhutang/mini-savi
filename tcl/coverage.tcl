#
######################################################
#
#  SaVi by Lloyd Wood (lloydwood@users.sourceforge.net),
#          Patrick Worfolk (worfolk@alum.mit.edu) and
#          Robert Thurman.
#
#  Copyright (c) 1997 by The Geometry Center.
#  Also Copyright (c) 2017 by Lloyd Wood.
#
#  This file is part of SaVi.  SaVi is free software;
#  you can redistribute it and/or modify it only under
#  the terms given in the file COPYRIGHT which you should
#  have received along with this file.  SaVi may be
#  obtained from:
#  http://savi.sourceforge.net/
#  http://www.geom.uiuc.edu/locate/SaVi
#
######################################################
#
# coverage.tcl
#
# $Id: coverage.tcl,v 1.176 2017/07/02 11:07:11 lloydwood Exp $

# Note: IM_W and IM_H are linked to C variables for the image size

proc coverage(init) {} {
    global coverage_flag no_access_flag print_access_flag tracks_flag map_flag \
	project_tracks_flag coverage_proj_flag coverage_angle_flag \
	coverage_flag_dismissed tracks_flag_dismissed no_access_flag_dismissed \
	Projection coverage_projection last_coverage_projection NUM_COLORS_PREV

    trace variable coverage_flag w flag_change
    trace variable no_access_flag w flag_change
    trace variable print_access_flag w flag_change
    trace variable tracks_flag w flag_change
    trace variable map_flag w flag_change
    trace variable project_tracks_flag w flag_change
    trace variable coverage_proj_flag w flag_change
    trace variable coverage_angle_flag w flag_change

    set project_tracks_flag 0
    set coverage_proj_flag 0
    set coverage_angle_flag 0
    set coverage_flag 0

    set NUM_COLORS_PREV 0

    # for texturemap demonstration - blue world in Geomview
    set no_access_flag 1
    set map_flag 1

    set coverage_flag_dismissed 1
    set tracks_flag_dismissed 1

    # this and popup menu match master list in globals.h
    set Projection(unprojected_mask) 0
    set Projection(unprojected) 1
    set Projection(cylindrical) 2
    set Projection(sinusoidal) 3
    set Projection(sinusoidal_90) 4
    set Projection(spherical) 5
    set Projection(spherical_90) 6

    set coverage_projection $Projection(unprojected)
    set last_coverage_projection $coverage_projection

    coverage(init_coldiv_coldec)
}

proc coverage(build) {} {
    global FONT COLOR COLDIV COLDEC coverage_flag no_access_flag tracks_flag \
	coverage_proj_flag coverage_angle_flag project_tracks_flag map_flag \
	coverage_projection coverage_angle im IM_W IM_H im_x_offset im_y_offset \
	coverage_flag_dismissed tracks_flag_dismissed no_access_flag_dismissed \
	coverage_display_center_longitude coverage_name \
	sun_flag Fisheye NUM_COLORS NUM_COLORS_PREV


    if {($IM_H == 0) || ($NUM_COLORS == 0)} {
	coverage_size(build)
	return
    }
    
    if {[eval window(raise) coverage]} return

    set coverage_name [build_Toplevel coverage]

    wm protocol $coverage_name WM_DELETE_WINDOW coverage(dismiss)

    coverage(title)

    build_Menubar $coverage_name mbar \
	{"Help" \
	     {"About this constellation..." "details(build)" "H"} \
	     {} \
	     {"Introduction to SaVi..." "helpfile(build) README"} \
	     {"Bugs in SaVi..." "helpfile(build) BUGS"} \
	     {"Texturemapping in Geomview..." "helpfile(build) README-COVERAGE-TEXTUREMAP"} \
	     {} \
             {"about SaVi..." "about(build)"} } \
	{"File" \
	     {"Load satellites..." "load(build)"} \
	     {"Save satellites as Tcl script..." "save(build)" "S"} \
             {} \
             {"Load new constellation TLEs from web..." "load_url_tle(build)"} \
	     {} \
	     {"Save map..." "coverage(save_map)" "M"} \
             {"Save animation..." "coverage_frames(build)"} \
	     {} \
	     {"Load color shades..." "coverage(load_colors)"} \
	     {"Save color shades..." "coverage(save_colors)"} \
	     {} \
	     {"Close window" "coverage(dismiss)" "W"} \
	     {"Quit" "main(exit)" "Q"} } \
	{"Edit" \
	     {"Simulation constants..." "params(build)"} } \
	{"Views" \
	     {"Satellites list..." "window(raise) main" "L"} \
	     {"Local fisheye..." "fisheye(build)" "F"} \
	     {} \
	     {"> Step forwards" "main(forw_step)"} \
	     {"< Step back" "main(back_step)"} \
	     {">> Forwards..." "main(forwards)"} \
	     {"<< Reverse..." "main(backwards)"} \
	     {"Freeze views" "main(stop)"} \
	     {} \
	     {"Refresh coverage view" "coverage(foot)"} \
	     {} \
	     {"Restart views at time zero" "main(restart)" "0"} } \
	{"Constellations"} \
	{"Rendering"} \
	{"Display"} \
	{"Shading" \
	     {"Change shading" "coverage(run_of_colors)"} \
	     {"Reverse shading" "coverage(reverse_colors)"} \
	     {} \
             {"Select random colors" "coverage(random_colors)"} \
	     {"Swap color swatches" "coverage(swap_color_diversity_decay)"} \
	     {"Shade between end colors" "coverage(blend_colors)"} \
	     {} \
	     {"SaVi color shades" "coverage(restore_colors)"} }

    main(constellations_menu) $coverage_name.mbar.b3

    constellations(rendering_menu) $coverage_name.mbar.b4

    build_options_menu $coverage_name.mbar.b5 \
	{"Show Earth map" map_flag} \
	{} \
	{"Show coverage" coverage_flag} \
	{"Use interval decay" no_access_flag} \
	{"Show sunlight" sun_flag} \
  {"Show fisheye viewpoint" fisheye_viewpoint_flag} \
	{} \
	{"Show satellites" tracks_flag} \
	{"Record ground tracks" project_tracks_flag}

    set cmd [build_CmdFrame $coverage_name cmd]

    set c "$cmd.c"
    # canvas counts from 0
    canvas $c -bg $COLOR(bg) -width $IM_W -height $IM_H
    pack $c -fill x -side top

    if {$coverage_display_center_longitude < 0} {
	set offset_longitude [expr $coverage_display_center_longitude + 360]
    } else {
	set offset_longitude $coverage_display_center_longitude
    }

    set im_x_offset [expr 2 + ($offset_longitude * -$IM_W/360)]
    set im_y_offset 2

    set im [image create photo im_coverage -width [expr $IM_W*2] -height $IM_H]
    $c create image [expr $IM_W + $im_x_offset] [expr $IM_H/2 + $im_y_offset] \
	-image $im

    # catch button click in map to get fisheye Lat/Long
    bind $c <Button-1> {coverage(lat_long) %x %y}
    bind $c <Control-Button-1> {coverage_set(lat_long) %x %y}

    bind $c <Button-2> {coverage(back_step)}
    bind $c <Button-3> {coverage(forw_step)}
    bind $c <Shift-Button-2> {coverage(backwards)}
    bind $c <Shift-Button-3> {coverage(forwards)}

    set cmdq [build_StdFrame $cmd q]
    build_Optionslist $cmdq olist \
	{"Show Earth map" map_flag}
    pack $cmdq.olist -side left
    build_IPopupMenu $cmdq p0 "projected as:" \
        coverage_projection coverage(map) {"unprojected land/sea" "unprojected" "cylindrical" "sinusoidal" "sinusoidal (90W)" "spherical" "spherical (90W)"}
    pack $cmdq.p0 -side left

    build_LabelEntryColumns $cmdq p1 \
	{text "" {"Clicked: "}} \
	{label "" {Fisheye(clicked_coords)}}

    pack $cmdq.p1 -side left
    pack $cmdq -anchor w

    set cmda [build_StdFrame $cmd a]
    build_Optionslist $cmda olist \
	{"Show satellites" tracks_flag} \
	{"Show coverage" coverage_flag} \
	{"Use interval decay" no_access_flag} \
	{"Show sunlight" sun_flag}
# this will give a numerical idea of coverage by latitude
#       {"Print access data" print_access_flag}
# this is hacked into utils.tcl instead.
#	{"Record ground tracks    and project forwards" project_tracks_flag}
    pack $cmda.olist -side left

    build_StdFrame $cmda ce
    build_StdFrame $cmda.ce cf
    label $cmda.ce.cf.0 -text "satellites in view\n(coverage diversity)" \
	    -font $FONT(label)
    pack $cmda.ce.cf.0 -side right -ipadx 2m -ipady 2m \
	    -padx 1m -pady 1m

    if {$NUM_COLORS != $NUM_COLORS_PREV} {
        coverage(init_colors)
    }

    for {set i 1} {$i < $NUM_COLORS} {incr i} {
        button $cmda.ce.cf.$i -text "$i" -font $FONT(fixed) -bg $COLDIV($i) \
	    -activebackground $COLDIV($i) -highlightbackground $COLDIV($i) \
	    -command "coverage(diversitycolor) $i" \
	    -relief groove
	pack $cmda.ce.cf.$i -expand 1 -fill x -side left -ipadx 2m -ipady 2m \
	    -padx 1m -pady 1m
    }
    button $cmda.ce.cf.$NUM_COLORS -text "$NUM_COLORS+" -font $FONT(fixed) \
	    -bg $COLDIV($NUM_COLORS) \
	    -activebackground $COLDIV($NUM_COLORS) \
	    -highlightbackground $COLDIV($NUM_COLORS) \
	    -command "coverage(diversitycolor) $NUM_COLORS" \
	    -relief groove
    pack $cmda.ce.cf.$NUM_COLORS -expand 1 -fill x -side left -ipadx 1m -ipady 2m \
	    -padx 1m -pady 1m

    set max_colors [expr $NUM_COLORS + 1]
    # label $cmda.ce.cf.$max_colors -text "satellites" -font $FONT(label)
    # pack $cmda.ce.cf.$max_colors -ipady 2m -expand 1

    pack $cmda.ce.cf -expand 1 -fill x -side left -ipadx 2m -ipady 2m \
	 -padx 1m -pady 1m
    pack $cmda.ce.cf -side top

    build_StdFrame $cmda.ce cg
    label $cmda.ce.cg.0 -text "intervals since view\n(coverage decay)" -font $FONT(label)
    pack $cmda.ce.cg.0 -side right -ipadx 2m -ipady 2m \
	    -padx 1m -pady 1m

    for {set i 1} {$i < $NUM_COLORS} {incr i} {
        button $cmda.ce.cg.$i -text "$i" -font $FONT(fixed) -bg $COLDEC($i) \
	    -activebackground $COLDEC($i) -highlightbackground $COLDEC($i) \
	    -command "coverage(decaycolor) $i" \
	    -relief groove
	pack $cmda.ce.cg.$i -expand 1 -fill x -side left -ipadx 2m -ipady 2m \
	    -padx 1m -pady 1m
    }
    button $cmda.ce.cg.$NUM_COLORS -text "$NUM_COLORS+" -font $FONT(fixed) \
	    -bg $COLDIV($NUM_COLORS) \
	    -activebackground $COLDIV($NUM_COLORS) \
	    -highlightbackground $COLDIV($NUM_COLORS) \
	    -command "coverage(diversitycolor) $NUM_COLORS" \
	    -relief groove
    pack $cmda.ce.cg.$NUM_COLORS -expand 1 -fill x -side right -ipadx 1m -ipady 2m \
	    -padx 1m -pady 1m

    # label $cmda.ce.cg.$max_colors -text "intervals" -font $FONT(label)
    # pack $cmda.ce.cg.$max_colors -ipady 2m -expand 1

    pack $cmda.ce.cg -expand 1 -fill x -side left -ipadx 2m -ipady 2m \
	 -padx 1m -pady 1m
    pack $cmda.ce.cg -side top

    pack $cmda.ce -side right -fill both

    pack $cmda -fill both

    # handles window being closed and reopened when sunlight on
    if {$sun_flag} {
	coverage(labels_show_sunlight)
    }

    build_LabelEntryColumns $cmd le2 \
	{lcheckbox "" project_tracks_flag} \
	{ientry "" {tracks_interval}} \
	{text "" {hours}}

    set cmdf [build_StdFrame $cmd f]
    build_IPopupMenu $cmdf p2 "Coverage angle describes:" \
	coverage_angle_flag coverage(foot) {"ground terminal mask elevation" "cones from satellites"}


    pack $cmdf.p2 -side left

    build_LabelEntryColumns $cmdf p3 \
	{slider " " { coverage(set_angle) } }

    pack $cmdf.p3 -side right -fill x -expand 1
    pack $cmdf

    set cmdb [build_StdFrame $cmd b]

    button $cmdb.b1 -font $FONT(button)
    $cmdb.b1 configure -text "Set angle:" \
		-command "coverage(foot)"
    pack $cmdb.b1 -side left

    build_LabelEntryColumns $cmdb le1 \
	{dentry "" {coverage_angle}} \
	{label "" {coverage}} \
	{label "" {mean_coverage}}

    bind $cmdb.le1.c0.0 <Return> coverage(foot)
    bind $cmdb.le1.c0.0 <Tab> coverage(foot)

    pack $cmdb.le1

    pack $cmdb -expand 1

    set cmdd [build_StdFrame $cmd d]

    build_LabelEntryColumns $cmdd le0 \
	{label "" {coverage_init_time}} \
	{label "" {coverage_time}} \
	{text "" "Time:"} \
	{label "" {time}}

    pack $cmdd.le0 -side left
    pack $cmdd -fill both -expand 1

    pack $cmd -fill both -side top -expand 1

    build_Buttonbar $coverage_name bb1 \
        {"Restart" coverage(restart)} \
        {"  <  " coverage(back_step)} \
        {"  >  " coverage(forw_step)} \
        {"  STOP  " coverage(stop)} \
        {"   < <   " coverage(backwards)} \
        {"   > >   " coverage(forwards)}

    set coverage_flag $coverage_flag_dismissed
    set tracks_flag $tracks_flag_dismissed

    coverage(update_colors)
}

proc constellations(rendering_menu) {submenu} {
    global geomview_module

    if {$geomview_module == 1} {
        build_options_menu $submenu \
            {"Real-time mode" realtime_flag} \
            {} \
            {"Show central body" earth_flag} \
	    {"Use simple Earth map" simple_earth_flag} \
            {"Use detailed Earth model" fancy_earth_flag} \
            {} \
            {"Show coverage in Geomview" geomview_dynamic_texture_flag}
    } else {
        build_options_menu $submenu \
            {"Real-time mode" realtime_flag}
    }
}

proc coverage(forw_step) {} {
    main(forw_step)
}

proc coverage(back_step) {} {
    main(back_step)
}

proc coverage(forwards) {} {
    global COLOR playbar

    if {([winfo exists .coverage])} {
	.coverage.bb1.b3 configure -bg $COLOR(bg) -highlightbackground $COLOR(bg)
	.coverage.bb1.b4 configure -bg $COLOR(bg) -highlightbackground $COLOR(bg)
	.coverage.bb1.b5 configure -bg $COLOR(sbg) -highlightbackground $COLOR(sbg)
    }
    set flag $playbar
    set playbar 1
    if {$flag == 0} {
	main(forwards)
    }
    set playbar 0
}

proc coverage(backwards) {} {
    global COLOR playbar

    if {([winfo exists .coverage])} {
	.coverage.bb1.b3 configure -bg $COLOR(bg) -highlightbackground $COLOR(bg)
	.coverage.bb1.b4 configure -bg $COLOR(sbg) -highlightbackground $COLOR(sbg)
	.coverage.bb1.b5 configure -bg $COLOR(bg) -highlightbackground $COLOR(bg)
    }
    set flag $playbar
    set playbar 1
    if {$flag == 0} {
	main(backwards)
    }
    set playbar 0
}

proc coverage(stop) {} {
    global COLOR playbar

    if {([winfo exists .coverage])} {
	.coverage.bb1.b3 configure -bg $COLOR(sbg) -highlightbackground $COLOR(sbg)
	.coverage.bb1.b4 configure -bg $COLOR(bg) -highlightbackground $COLOR(bg)
	.coverage.bb1.b5 configure -bg $COLOR(bg) -highlightbackground $COLOR(bg)
    }
    set flag $playbar
    set playbar 1
    if {$flag == 0} {
	main(stop)
    }
    set playbar 0
}

proc coverage(set_angle) v {
    global coverage_angle

    # this is a hack to prevent resetting to zero when coverage window is reopened.
    if {$v == 0} return

    set coverage_angle [expr floor($v/100.0*90)]

    coverage(update_angle)
}

proc coverage(update_angle) {} {
    global coverage_flag coverage_angle cones_flag footprints_flag

    set coverage_angle [expr abs($coverage_angle)]
    if {$coverage_angle > 90} {
	set coverage_angle 90
    }
    
    coverage(update_decay)

    if {$coverage_flag == 1} {
	set coverage_flag 0
	set coverage_flag 1
    }

    coverage(update)

    geomview(update_cones_footprints)

    fisheye(update)
}

proc coverage(restart) {} {

    coverage(stop)
    satellites RESET

}

proc coverage(init_colors) {} {
    global NUM_COLORS
    global DIV DEC

    set DIV(0) [expr 0xffffff]
    set DIV(1) [expr 0xffff00]
    set DIV($NUM_COLORS) [expr 0xff0000]

    set DEC(0) [expr 0xffffff]
    set DEC(1) [expr 0x0000ff]
    set DEC($NUM_COLORS) [expr 0x00008f]

    coverage(blend_color_values) 1
}

proc coverage(init_coldiv_coldec) {} {
    global COLDIV COLDEC

    # this is just to name our variables.

    set COLDIV(0) #ffffff
    set COLDIV(1) #ffffff
    set COLDIV(2) #ffffff
    set COLDIV(3) #ffffff
    set COLDIV(4) #ffffff
    set COLDIV(5) #ffffff
    set COLDIV(6) #ffffff
    set COLDIV(7) #ffffff
    set COLDIV(8) #ffffff
    set COLDIV(9) #ffffff
    set COLDIV(10) #ffffff
    set COLDIV(11) #ffffff
    set COLDIV(12) #ffffff
    set COLDIV(13) #ffffff
    set COLDIV(14) #ffffff
    set COLDIV(15) #ffffff
    set COLDIV(16) #ffffff
    set COLDIV(17) #ffffff
    set COLDIV(18) #ffffff
    set COLDIV(19) #ffffff

    set COLDEC(0) #ffffff
    set COLDEC(1) #ffffff
    set COLDEC(2) #ffffff
    set COLDEC(3) #ffffff
    set COLDEC(4) #ffffff
    set COLDEC(5) #ffffff
    set COLDEC(6) #ffffff
    set COLDEC(7) #ffffff
    set COLDEC(8) #ffffff
    set COLDEC(9) #ffffff
    set COLDEC(10) #ffffff
    set COLDEC(11) #ffffff
    set COLDEC(12) #ffffff
    set COLDEC(13) #ffffff
    set COLDEC(14) #ffffff
    set COLDEC(15) #ffffff
    set COLDEC(16) #ffffff
    set COLDEC(17) #ffffff
    set COLDEC(18) #ffffff
    set COLDEC(19) #ffffff
}

proc coverage(labels_show_land_sea) {} {
    global NUM_COLORS coverage_projection Projection

    if {$coverage_projection != $Projection(unprojected_mask)} {
	return
    }

    if [winfo exists .coverage] {
	set last [expr $NUM_COLORS]
	for {set i 1} {$i < $last} {incr i} {
	    set j [expr $i-1]
	    .coverage.cmd.a.ce.cf.$i configure -text $j
	    .coverage.cmd.a.ce.cg.$i configure -text $j
	}
	
	set j [expr $last - 1]
	.coverage.cmd.a.ce.cf.$NUM_COLORS configure -text "$j+"
	.coverage.cmd.a.ce.cg.$NUM_COLORS configure -text "$j+"
	.coverage.cmd.a.ce.cf.0 configure -text "satellites in view\n(coverage on land)"
	.coverage.cmd.a.ce.cg.0 configure -text "satellites in view\n(coverage on sea)"
    }
}

proc coverage(labels_show_diversity) {} {
    #top line only
    global NUM_COLORS coverage_projection Projection

    if {$coverage_projection == $Projection(unprojected_mask)} {
	return
    }

    if [winfo exists .coverage] {
	set last [expr $NUM_COLORS ]
	for {set i 1} {$i < $last} {incr i} {
	    .coverage.cmd.a.ce.cf.$i configure -text $i
	}
	
	.coverage.cmd.a.ce.cf.$NUM_COLORS configure -text "$NUM_COLORS+"
	.coverage.cmd.a.ce.cf.0 configure -text "satellites in view\n(coverage diversity)"
    }
}

proc coverage(labels_show_decay) {} {
    global NUM_COLORS coverage_projection Projection

    if {$coverage_projection == $Projection(unprojected_mask)} {
	return
    }

    if [winfo exists .coverage] {
	set last [expr $NUM_COLORS]
	for {set i 1} {$i < $last} {incr i} {
	    .coverage.cmd.a.ce.cg.$i configure -text $i
	}

	.coverage.cmd.a.ce.cg.$NUM_COLORS configure -text "$NUM_COLORS+"
	.coverage.cmd.a.ce.cg.0 configure -text "intervals since view\n(coverage decay)"
    }
}

proc coverage(labels_show_sunlight) {} {
    global NUM_COLORS coverage_projection Projection

    if {$coverage_projection == $Projection(unprojected_mask)} {
	return
    }

    if [winfo exists .coverage] {
	set last [expr $NUM_COLORS - 1]
	for {set i 1} {$i < $last} {incr i} {
	    .coverage.cmd.a.ce.cg.$i configure -text ""
	}

	if {$last > 0 } {
	    .coverage.cmd.a.ce.cg.$last configure -text "day"
	}
	.coverage.cmd.a.ce.cg.$NUM_COLORS configure -text "night"
	.coverage.cmd.a.ce.cg.0 configure -text "sunlight\n "
    }
}


proc coverage(update_wells) {} {
global COLDIV COLDEC NUM_COLORS COLOR
global coverage_projection Projection coverage_flag no_access_flag sun_flag

    if {$coverage_projection == $Projection(unprojected_mask)} {
	if {$no_access_flag == 0} {
	    set j 2
	} else {
	    set j 0
	}
    } elseif {$sun_flag == 1} {
	set j [expr $NUM_COLORS - 1]
    } else {
	set j 0
    }

    for {set i 1} {$i <= $NUM_COLORS} {incr i} {

	# set -highlightbackground to black for frame surround on Mac OS X
	# to hint that buttons are not active and cannot set colours,
	# since the buttons always look active.

	if {$coverage_flag == 0} {
	  .coverage.cmd.a.ce.cf.$i configure -bg $COLOR(bg) \
	      -activebackground $COLOR(bg) -highlightbackground black \
	      -relief flat \
	      -command ""
	} else {
	   .coverage.cmd.a.ce.cf.$i configure -bg $COLDIV($i) \
	      -activebackground $COLDIV($i) -highlightbackground $COLDIV($i) \
	      -relief groove \
	      -command "coverage(diversitycolor) $i"
	}

	if {($coverage_flag == 0) || (($no_access_flag == 0)&&($coverage_projection != $Projection(unprojected_mask))) || ($i < $j)} {
	  .coverage.cmd.a.ce.cg.$i configure -bg $COLOR(bg) \
	       -activebackground $COLOR(bg) -highlightbackground black \
	       -relief flat \
	       -command ""
	} else {
	   .coverage.cmd.a.ce.cg.$i configure -bg $COLDEC($i) \
	       -activebackground $COLDEC($i) -highlightbackground $COLDEC($i) \
	       -relief groove \
	       -command "coverage(decaycolor) $i"
	}
    }
}

proc coverage(update_colors) {} {
    global DIV DEC
    global COLDIV COLDEC NUM_COLORS

    for {set i 0} {$i <= $NUM_COLORS} {incr i} {
	set COLDIV($i) [eval format "#%06x" $DIV($i)]
	set COLDEC($i) [eval format "#%06x" $DEC($i)]
    }
    satellites COLOR_UPDATE

    # if texturemapping, need to update sphere map
    geomview(update_texture)

    coverage(update_wells)
    coverage(update)
}

proc coverage(swap_color_diversity_decay) {} {
    global DIV DEC
    global NUM_COLORS

    for {set i 1} {$i <= $NUM_COLORS} {incr i} {
	set a $DIV($i)
	set DIV($i) $DEC($i)
	set DEC($i) $a
    }
    coverage(update_colors)
}

proc coverage(random_color_values) {} {
    global DIV DEC coverage_projection Projection
    global NUM_COLORS

    global coverage_flag no_access_flag sun_flag

    for {set i 1} {$i <= $NUM_COLORS} {incr i} {
	if {$coverage_flag == 1} {
	    set DIV($i) [random_color]
	}
	if {(($coverage_projection == $Projection(unprojected_mask)) || ($no_access_flag == 1)) && ($sun_flag == 0)} {
	    set DEC($i) [random_color]
	}
    }
}

proc coverage(random_colors) {} {
    coverage(random_color_values)
    coverage(update_colors)
}

proc coverage(restore_colors) {} {

    coverage(init_colors)

    coverage(update_colors)
}

proc coverage(run_of_colors) {} {
    global DIV DEC
    global NUM_COLORS coverage_flag no_access_flag sun_flag

    if {$coverage_flag == 1} {
       set DIV(1) [random_color]
       set DIV($NUM_COLORS) [random_color]
    }

    if {($no_access_flag == 1) && ($sun_flag == 0)} {
      set DEC(1) [random_color]
      set DEC($NUM_COLORS) [random_color]
    }

    coverage(blend_colors)
}

proc coverage(reverse_color_values) {} {
    global DIV DEC NUM_COLORS coverage_flag no_access_flag sun_flag

    if {$NUM_COLORS % 2 == 0} {
	# even
	set flip [expr $NUM_COLORS / 2]
    } else {
	# odd
	set flip [expr ($NUM_COLORS-1) / 2]
    }

    for {set i 1} {$i <= $flip} {incr i} {
	set high [expr $NUM_COLORS + 1 - $i]
	if {$coverage_flag == 1} {
	    set j $DIV($i)
	    set DIV($i) $DIV($high)
	    set DIV($high) $j
	}

	if {($no_access_flag == 1) && ($sun_flag == 0)} {
	    set j $DEC($i)
	    set DEC($i) $DEC($high)
	    set DEC($high) $j
	}
    }
}

proc coverage(reverse_colors) {} {
    coverage(reverse_color_values)
    coverage(update_colors)
}

proc coverage(blend_color_values) {force} {
    global DIV DEC
    global NUM_COLORS coverage_flag no_access_flag

    if {$NUM_COLORS < 3} {
	# no blending required
	return
    }

    set interval [expr $NUM_COLORS - 1]

    if {$force ||($coverage_flag == 1)} {
	set DIV1B [expr $DIV(1) % 256]
	set DIV1G [expr ($DIV(1) - $DIV1B)/256 % 256]
	set DIV1R [expr ($DIV(1) - $DIV1B - $DIV1G * 256) / 65536]

	set DIVlast $DIV($NUM_COLORS)
	set DIVlastB [expr $DIVlast % 256]
	set DIVlastG [expr ($DIVlast - $DIVlastB)/256 % 256]
	set DIVlastR [expr ($DIVlast - $DIVlastB - $DIVlastG * 256) / 65536]

	set DIVRinc [expr double($DIVlastR - $DIV1R) / $interval]
	set DIVGinc [expr double($DIVlastG - $DIV1G) / $interval]
	set DIVBinc [expr double($DIVlastB - $DIV1B) / $interval]

	for {set i 2} {$i < $NUM_COLORS} {incr i} {
	    set j [expr $i - 1]
	    set DIV($i) [expr $DIV(1) + $j * (int($DIVRinc) * 65536 + int($DIVGinc) * 256 + int($DIVBinc))]
	}
    }

    if {$force ||($no_access_flag == 1)} {
	set DEC1B [expr $DEC(1) % 256]
	set DEC1G [expr ($DEC(1) - $DEC1B)/256 % 256]
	set DEC1R [expr ($DEC(1) - $DEC1B - $DEC1G * 256) / 65536]

	set DEClast $DEC($NUM_COLORS)
	set DEClastB [expr $DEClast % 256]
	set DEClastG [expr ($DEClast - $DEClastB)/256 % 256]
	set DEClastR [expr ($DEClast - $DEClastB - $DEClastG * 256) / 65536]

	set DECRinc [expr double($DEClastR - $DEC1R) / $interval]
	set DECGinc [expr double($DEClastG - $DEC1G) / $interval]
	set DECBinc [expr double($DEClastB - $DEC1B) / $interval]

	for {set i 2} {$i < $NUM_COLORS} {incr i} {
	    set j [expr $i - 1]
	    set DEC($i) [expr $DEC(1) + $j * (int($DECRinc) * 65536 + int($DECGinc) * 256 + int($DECBinc))]
	}
    }
}

proc coverage(blend_colors) {} {
    coverage(blend_color_values) 0
    coverage(update_colors)
}

proc coverage(save_colors) {} {
    global DIV DEC NUM_COLORS
    global user_file_dir

    set types {
        {"SaVi coverage color shades"  {.shades} }
    }

    set filename [tk_getSaveFile -initialdir "$user_file_dir" -filetypes $types \
	          -title "SaVi: save coverage color shades" ]

    if {"$filename" == ""} return

    if {![string_ends "$filename" ".shades"]} {
	set filename "$filename.shades"
    }

    # open file
    set f [open "$filename" w]
    if {$f == ""} {
	puts stderr "\nSaVi: couldn't save coverage color shades to $filename"
	return
    }

    # write colors
    puts $f "\# SaVi saved coverage color shades"
    puts $f "\# http://savi.sourceforge.net/"

    for {set i 1} {$i <= $NUM_COLORS} {incr i} {
      puts $f "set DIV($i) $DIV($i)"
      puts $f "set DEC($i) $DEC($i)"
    }

    close $f

    puts stderr "\nSaVi: saved coverage color shades to $filename"
}

proc coverage(load_colors) {} {
   global user_file_dir

    set types {
        {"SaVi coverage color shades"  {.shades} }
    }

    set filename [tk_getOpenFile -initialdir "$user_file_dir" -filetypes $types \
	          -parent .main -title "SaVi: load coverage color shades" ]

    if {"$filename" == ""} return

    coverage(source_colors) "$filename"
    coverage(update_colors)

}

proc coverage(source_colors) {filename} {
    global DIV DEC

    source "$filename"
}

proc coverage(diversitycolor) {n} {
    global DIV
    global COLDIV coverage_flag

    if {$coverage_flag == 0} return

    if {$n == "1"} {
	set s ""
    } else {
	set s "s"
    }

    set color [tk_chooseColor -initialcolor $COLDIV($n) -parent .coverage \
	    -title "SaVi: diversity color for $n satellite$s visible"]
    if {($color != "") && ($color != $COLDIV($n))} {
	set COLDIV($n) $color
	.coverage.cmd.a.ce.cf.$n configure -bg $COLDIV($n) \
		-highlightbackground $COLDIV($n) -activebackground $COLDIV($n)
	set value [string range $COLDIV($n) 1 6]
	set DIV($n) [expr 0x$value]
    }
    satellites COLOR_UPDATE
    coverage(update_decay)
}

proc coverage(decaycolor) {n} {
    global DEC coverage_projection Projection
    global COLDEC no_access_flag

    if {($coverage_projection != $Projection(unprojected_mask)) &&
	($no_access_flag == 0)} return

    if {$n == "1"} {
	set s ""
    } else {
	set s "s"
    }

    set color [tk_chooseColor -initialcolor $COLDEC($n) -parent .coverage \
	    -title "SaVi: decay color for $n interval$s since covered"]
    if {($color != "") && ($color != $COLDEC($n))} {
	set COLDEC($n) $color
	.coverage.cmd.a.ce.cg.$n configure -bg $COLDEC($n) \
		-highlightbackground $COLDEC($n) -activebackground $COLDEC($n)
	set value [string range $COLDEC($n) 1 6]
	set DEC($n) [expr 0x$value]
    }
    satellites COLOR_UPDATE
    coverage(update_decay)
}

proc coverage(save_map_handler) {filename} {
    global im IM_H IM_W

    # supported in Tcl 8.5 and 8.6
    if {[string_ends "$filename" ".gif"] } {
        set ext gif
    }

    # not supported in Tcl 8.5. Uncertain 8.6 support.
    if {[string_ends "$filename" ".png"] } {
        set ext png 
    }

    if {[string_ends "$filename" ".ppm"] } {
        set ext ppm
    }

    $im write "$filename" -format $ext -from 0 0 $IM_W $IM_H
    puts stderr "\nSaVi: saved map as $ext to $filename"
}

proc coverage(save_map) {} {
    global user_file_dir

    # {"SaVi coverage map (png)" {.png} }
    # gives two-color images in 8.6 only, doesn't work in 8.5,
    # and produces larger filesizes than gif.

    # {"SaVi coverage map (ppm)" {.ppm} }
    # works consistently, but is uncompressed.
    set types {
        {"SaVi coverage map (gif)" {.gif} } \
	{"SaVi coverage map (gzipped)" {.ppm.gz} } \
    }

    set filename [tk_getSaveFile -initialdir "$user_file_dir" -filetypes $types \
	          -parent .coverage -title "SaVi: save coverage map" ]

    if {"$filename" == ""} return

    set length [string length "$filename"]

    if {[string_ends "$filename" ".png"] || [string_ends "$filename" ".gif"]} {
       coverage(save_map_handler) $filename
       return
    }

    if {[string_ends "$filename" ".gz"]} {
	set filename [string range "$filename" 0 [expr $length - 4]]
    }

    if {![string_ends $filename ".ppm"]} {
	set filename "$filename.ppm"
    }

    satellites COVERAGE_SAVE_MAP "$filename"

    set gzip gzip

    if {[catch {eval exec $gzip -9 "$filename"}]} {
        puts stderr "\nSaVi: saved map as raw ppm to $filename"
	puts stderr "      failed to compress map with $gzip."
        return
    }
    puts stderr "\nSaVi: saved $gzip-compressed map to $filename.gz"
}

proc coverage(dismiss) {} {
    global NUM_COLORS NUM_COLORS_PREV coverage_flag tracks_flag \
	   coverage_flag_dismissed tracks_flag_dismissed

    set coverage_flag_dismissed $coverage_flag
    set tracks_flag_dismissed $tracks_flag

    # turn off flags to avoid computing drawing when window not shown
    set coverage_flag 0
    set tracks_flag 0

    # reset colours to force being asked again.
    set NUM_COLORS_PREV $NUM_COLORS
    set NUM_COLORS 0

    destroy .coverage
}

proc coverage(update_decay) {} {
    global no_access_flag

    if {$no_access_flag == 0} return

    set flag $no_access_flag
    set no_access_flag 1
    if {$flag == 0} {
	set no_access_flag 0
    }
}

proc coverage(update) {} {
    global coverage_flag tracks_flag

    if {![winfo exists .coverage]} return

    coverage(update_decay)

    set flag $coverage_flag
    set coverage_flag 1
    if {$flag == 0} {
	set coverage_flag 0
    }

    set flag $tracks_flag
    set tracks_flag 1
    if {$flag == 0} {
	set tracks_flag 0
    }

    update
}

proc coverage(lat_long) {x y} {
    global Fisheye coverage_projection IM_W IM_H im_x_offset im_y_offset \
	   Longitude_Center_Line PI coverage_display_center_longitude Projection

    set x [expr $x-$im_x_offset]
    set y [expr $y-$im_y_offset]

    set HALF_X [expr $IM_W/2]
    set HALF_Y [expr $IM_H/2]

    set off_map 0

    if {$coverage_projection == $Projection(cylindrical)} {
	# cylindrical
	set long [expr (($x%$IM_W)-$HALF_X)/double($IM_W)*360.0+$Longitude_Center_Line]
	set y [expr 1-$y/double($IM_H)*2]
	if {$y < -1} {
	    set y -1
	} elseif {$y > 1} {
	    set y 1
	}
	set lat [expr asin($y)/$PI*180.0]
    } elseif {($coverage_projection == $Projection(unprojected))||($coverage_projection == $Projection(unprojected_mask))} {
	# unprojected
	set long [expr (($x%$IM_W)-$HALF_X)/double($IM_W)*360.0+$Longitude_Center_Line]
	set y [expr 1-$y/double($IM_H)*2]
	if {$y < -1} {
	    set y -1
	} elseif {$y > 1} {
	    set y 1
	}
	set lat [expr $y*90]
    } elseif {$coverage_projection == $Projection(sinusoidal)} {
	# sinusoidal 0W
	set lat [expr ($HALF_Y-$y)/double($HALF_Y)*90.0]
	if {abs($lat) == 90} {
	    set long 0
	} else {
	    set long [expr \
	    (($x%$IM_W)-$HALF_X)/double($HALF_X)/cos($lat/180.0*$PI)*180]
	}
	if {$long > 180} {
	    set long 180
	    set off_map 1
	} elseif {$long < -180} {
	    set long 180
	    set off_map 1
	}
    } elseif {$coverage_projection == $Projection(sinusoidal_90)} {
	# sinusoidal 90W
	set lat [expr ($HALF_Y-$y)/double($HALF_Y)*90.0]
	if {abs($lat) == 90} {
	    set long 0
	} else {
	    set long [expr \
	    (($x%$IM_W)-$HALF_X)/double($HALF_X)/cos($lat/180.0*$PI)*180 + $Longitude_Center_Line]
	}
	# below presumes that LCR is -90; it depends on the ppm map anyway.
	if {$long > ($Longitude_Center_Line+180)} {
	    set long [expr $Longitude_Center_Line+180]
	    set off_map 1
	} elseif {$long < ($Longitude_Center_Line-180)} {
	    set long [expr $Longitude_Center_Line+180]
	    set off_map 1
	}
    } else {
	# spherical 0W/90W
	set disty [expr abs($y-$HALF_Y)]
	set laty [expr 1-$y/double($IM_H)*2]
	if {$laty < -1} {
	    set laty -1
	} elseif {$laty > 1} {
	    set laty 1
	}
	set lat [expr asin($laty)/$PI*180.0]

	set x [expr $x%$IM_W]
	if {$x > $HALF_X} {
	    set distx [expr $x - $IM_W*0.75]
	    set first_sphere 0
	} else {
	    set distx [expr $x - $IM_W*0.25]
	    set first_sphere 1
	}
	if {(sqrt($distx*$distx + $disty*$disty)) > $HALF_Y} {
	    set off_map 1
	}

        if {$coverage_projection == $Projection(spherical)} {
	    if {$first_sphere == 1} {
	        if {$off_map == 1} {
	            if {$distx > 0} {
	                set long 0
	            } else {
	                set long 180
	            }
	        } else {
	            # compute value on map
		    set long [expr -90 + 90*$distx/($HALF_X/2.0)*cos($lat*$PI/180)]
	        }
	    } else {
	        if {$off_map == 1} {
	            if {$distx > 0} {
	                set long 180
	            } else {
	                set long 0
                    }
                } else {
	            # compute value on map
	            set long [expr 90 + 90*$distx/($HALF_X/2.0)*cos($lat*$PI/180)]
	        }
            }
	} elseif {$coverage_projection == $Projection(spherical_90)} {
            if {$first_sphere == 1} {
	        if {$off_map == 1} {
	            if {$distx > 0} {
	                set long -90
	            } else {
	                set long 90
	            }
	        } else {
	            # compute value on map
	            set long [expr 180 + 90*$distx/($HALF_X/2.0)*cos($lat*$PI/180)]
	        }
	    } else {
	        if {$off_map == 1} {
	            if {$distx > 0} {
	                set long 90
	            } else {
	                set long -90
	            }
	        } else {
	            # compute value on map - was
	            set long [expr 0 + 90*$distx/($HALF_X/2.0)*cos($lat*$PI/180)]
	        }
	    }
	}
    }

    if {$off_map == 0} {
	if {$long <= -180} {
	    set long [expr $long+360]
	} elseif {$long > 180} {
	    set long [expr $long-360]
	}
    }

    set Fisheye(new_lat_degrees) [expr {$lat<0 ? -int(-$lat) : int($lat)} ]
    set Fisheye(new_lat_minutes) [expr abs($lat-$Fisheye(new_lat_degrees))*60.0]
    set Fisheye(new_long_degrees) [expr {$long<0 ? -int(-$long) : int($long)} ]
    set Fisheye(new_long_minutes) [expr abs($long-$Fisheye(new_long_degrees))*60.0]

    if {$lat >= 0} {
	set latdir "N"
    } else {
	set latdir "S"
    }

    if {$long >=0} {
	set longdir "E"
    } else {
	set longdir "W"
    }

    set Fisheye(clicked_coords) "($Fisheye(new_lat_degrees)$latdir, $Fisheye(new_long_degrees)$longdir)"

    # We could call fisheye(apply) here to set the coordinates directly,
    # but it's more useful to just know where you've clicked without also
    # changing the fisheye location.

}

proc coverage_set(lat_long) {x y} {

    coverage(lat_long) $x $y
    fisheye(apply)
}

proc coverage(foot) {} {
    global coverage_angle coverage_flag

    if {$coverage_angle > 90} {
      set coverage_angle 90
    } elseif {$coverage_angle < 0} {
      set coverage_angle 0
    }

    coverage(update_angle)
}

proc coverage(title) {} {
    global coverage_name coverage_projection Projection

    if {$coverage_projection == $Projection(cylindrical)} {
	build_Title $coverage_name "view of coverage on cylindrical Earth map"
    } elseif {$coverage_projection == $Projection(unprojected)} {
	build_Title $coverage_name "view of coverage on unprojected (equirectangular) Earth map"
    } elseif {$coverage_projection == $Projection(unprojected_mask)} {
	build_Title $coverage_name "view of coverage on unprojected (equirectangular) colorful Earth map"
    } elseif {$coverage_projection == $Projection(sinusoidal)} {
	build_Title $coverage_name "view of coverage on sinusoidal Earth map"
    } elseif {$coverage_projection == $Projection(sinusoidal_90)} {
	build_Title $coverage_name "view of coverage on sinusoidal Earth map (centred on 90W for Americas)"
    } elseif {$coverage_projection == $Projection(spherical)} {
	build_Title $coverage_name "view of coverage on east/west hemispheres (equatorial orthographic maps)"
    } elseif {$coverage_projection == $Projection(spherical_90)} {
	build_Title $coverage_name "view of coverage from 180/0 deg latitudes (equatorial orthographic maps)"
    }
}

proc coverage(map) {} {
    global coverage_proj_flag coverage_projection last_coverage_projection \
	Projection sun_flag

    if {$coverage_projection == $last_coverage_projection} return

    coverage(title)

    set coverage_proj_flag [expr 1 - $coverage_proj_flag]

    if {($coverage_projection == $Projection(unprojected_mask))} {
	coverage(labels_show_land_sea)
    } else {
	if {$sun_flag} {
	    coverage(labels_show_diversity)
	    coverage(labels_show_sunlight)
	} else {
	    coverage(labels_show_diversity)
	    coverage(labels_show_decay)
	}
    }

    set last_coverage_projection $coverage_projection
    coverage(update)

        # this is a hack to reset Earth while dynamic texturemapping,
    # if we move from cylindrical/unprojected projections supporting
    # texturemapping to other projections that don't.
    geomview(update_texture)
}
