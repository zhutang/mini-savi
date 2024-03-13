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
# main.tcl
#
# $Id: main.tcl,v 1.166 2017/01/02 11:30:45 lloydwood Exp $

proc main(build) {} {
    global main_name COLOR FONT TITLE geomview_module last_filename PLATFORM

    if {[eval window(raise) main]} return

    set main_name [build_Toplevel main]

    wm protocol $main_name WM_DELETE_WINDOW main(exit)

    wm title $main_name "$TITLE - satellite constellation visualization"

    build_Menubar $main_name mbar \
	{"Help" \
	     {"About this constellation..." "details(build)" "H"} \
	     {} \
	     {"Introduction to SaVi..." "helpfile(build) README"} \
	     {"Bugs in SaVi..." "helpfile(build) BUGS"} \
	     {"Texturemapping in Geomview..." "helpfile(build) README-COVERAGE-TEXTUREMAP"} \
	     {} \
             {"about SaVi..." "about(build)"} } \
	{"File"} \
	{"Edit" \
	     {"Simulation constants..." "params(build)"} \
	     {} \
	     {"Edit selected satellite..." "edit(build)" "E"} \
	     {"New satellite..." "main(new)" "N"} \
	     {} \
	     {"Unselect" "main(unselect_all)" } \
	     {"Copy" "main(copy)" "C"} \
	     {"Cut" "main(delete)" "X"} } \
	{"Views"} \
	{"Constellations"} \
	{"Rendering"}

    if {$geomview_module == 0} {
	build_Menu $main_name.mbar.b0 \
	    {"Load more satellites..." "load(build)"} \
	    {"Save satellites as Tcl script..." "save(build)" "S"} \
	    {} \
            {"Load new constellation TLEs from web..." "load_url_tle(build)"} \
	    {} \
	    {"Quit" "main(exit)" "Q"}
	build_Menu $main_name.mbar.b2 \
	    {"Global coverage..." "coverage(build)" "G"} \
	    {"Local fisheye..." "fisheye(build)" "F"} \
	    {} \
	    {"> Step forwards" "main(forw_step)"} \
	    {"< Step back" "main(back_step)"} \
	    {">> Forwards..." "main(forwards)"} \
	    {"<< Reverse..." "main(backwards)"} \
	    {"Freeze views" "main(stop)"} \
	    {} \
	    {"Restart views at time zero" "main(restart)" "0"}
    } else {
	build_Menu $main_name.mbar.b0 \
	    {"Load satellites..." "load(build)"} \
	    {"Save satellites as Tcl script..." "save(build)" "S"} \
            {} \
            {"Load new constellation TLEs from web..." "load_url_tle(build)"} \
	    {} \
	    {"Save Geomview camera screenshots..." "no_frames(build)"} \
	    {"Save Geomview print rendering..." "snapshot(build)"} \
	    {} \
	    {"Quit" "main(exit)" "Q"}
	build_Menu $main_name.mbar.b2 \
	    {"Global coverage..." "coverage(build)"} \
	    {"Local fisheye..." "fisheye(build)"} \
	    {} \
	    {"Geomview background color..." "geomview(choose_background_color)"} \
	    {"View north pole in Geomview" "geomview(camera_north_pole)"} \
	    {"View south pole in Geomview" "geomview(camera_south_pole)"} \
	    {} \
	    {"> Step forwards" "main(forw_step)"} \
	    {"< Step back" "main(back_step)"} \
	    {">> Forwards..." "main(forwards)"} \
	    {"<< Reverse..." "main(backwards)"} \
	    {"Freeze views" "main(stop)"} \
	    {} \
	    {"Restart views at time zero" "main(restart)"}
    }

    main(constellations_menu) $main_name.mbar.b3

    main(rendering_menu) $main_name.mbar.b4

    set cmd [build_StdFrame $main_name cmd]

    # needs to be 78 on old 10.4 Mac with supplied Tcl/Tk to avoid crash.
    set WIDTH 99
    if {$PLATFORM == 1} {
        set WIDTH 78
    }

    label $cmd.l -text \
	"no.  semi-major axis  eccentricity  inclination  longitude asc. node  arg. periapsis  time to periapsis  satellite name" \
	-font $FONT(label)
    listbox $cmd.lb -yscrollcommand "$cmd.scroll set" \
	-height 2 -width $WIDTH -selectmode single \
	-font $FONT(fixed) -bg $COLOR(entry)
    pack $cmd.l -side top -anchor w
    pack $cmd.l -fill none
    pack $cmd.lb -side left
    pack $cmd.lb -fill both -expand 1
    bind $cmd.lb <Button-1> {main(select) [%W nearest %y]}
    bind $cmd.lb <Double-Button-1> {main(dblselect) [%W nearest %y]}
    bind $cmd.lb <Shift-Button-1> {main(rangeselect) [%W nearest %y]}
    bind $cmd.lb <Shift-Double-Button-1> {main(rangedblselect) [%W nearest %y]}
    bind $cmd.lb <Control-Button-1> {main(itemselect) [%W nearest %y]}
    bind $cmd.lb <Control-Double-Button-1> {main(itemdblselect) [%W nearest %y]}
    bind $cmd.lb <Delete> {main(delete)}
    bind $cmd.lb <Insert> {main(new)}
    scrollbar $cmd.scroll -command "$cmd.lb yview"
    pack $cmd.scroll -side right -fill y

    pack $cmd -fill both -expand 1

    set cmd0 [build_CmdFrame $main_name cmd0]

    build_LabelEntryColumns $cmd0 le0 \
        {dentry "" {delta_t}} \
	{text "" {"seconds per simulation interval."}} \
	{label "" {time}} \
	{label "" {last_filename}}

    pack $cmd0 -fill x

    build_Buttonbar $main_name bb1 \
        {"Restart" {main(restart)}} \
        {"  <  " {main(back_step)}} \
        {"  >  " {main(forw_step)}} \
        {"  STOP  " main(stop)} \
        {"   < <   " main(backwards)} \
        {"   > >   " main(forwards)}

    main(start_satellites)

    # show help menu
    details(build)

}

proc main(start_satellites) {} {
    global first_filename last_filename source_comments comments_text comments_file details_source

    if { [string compare "$first_filename" "NULL"] } {
        main(load_file) "$first_filename"
    } else {
      set last_filename "a LEO satellite."
      set source_comments "An example satellite in a low equatorial orbit."
      set comments_text ""
      set comments_file ""
      set details_source ""
    }
}

proc main(reset_title) {} {
  global TITLE main_name

  wm title $main_name "$TITLE - satellite constellation visualization"
}

proc main(title) {filename} {
  global TITLE main_name

  wm title $main_name "$TITLE - $filename - satellite constellation visualization"
}

proc main(constellations_menu) {submenu} {

    build_Menu $submenu \
	{"Empty space..." "empty(build)" "M"} \
	{"Ballard rosette..." "rosette(build)"} \
	{"Crude star..." "star(build)"} \
	{} \
	{"Clarke geostationary" "main(constellation) clarke.tcl"} \
	{"Draim tetrahedral" "main(constellation) draim-4.tcl"} \
	{"Molnya elliptical" "main(constellation) molnya.tcl"} \
	{"Tundra elliptical" "main(constellation) tundra.tcl"} \
	{"Quasi-geostationary" "main(constellation) quasi-geo.tcl"} \
	{} \
	{"Globalstar" "main(constellation) globalstar.tcl"} \
	{"Iridium" "main(constellation) iridium-66.tcl"} \
	{"Orbcomm" "main(constellation) orbcomm.tcl"} \
	{"Sirius Radio elliptical" "main(constellation) sirius-radio.tcl"} \
	{"DMC disaster monitoring" "main(constellation) dmc.tcl"} \
	{"RapidEye" "main(constellation) rapideye.tcl"} \
	{} \
	{"GPS" "main(constellation) gps.tcl"} \
	{"Glonass" "main(constellation) glonass.tcl"} \
	{"Galileo" "main(constellation) galileo.tcl"} \
	{} \
        {"O3b Networks" "main(constellation) o3b-networks.tcl"} \
        {"Orblink" "main(constellation) orblink.tcl"} \
        {"LEq0" "main(constellation) leqo.tcl"} \
        {} \
        {"ViaSat NGSO" "main(constellation) viasat-ngso.tcl"} \
	{"O3b full global coverage" "main(constellation) o3b-networks-full.tcl"} \
	{"LeoSat (108)" "main(constellation) leosat.tcl"} \
	{"OneWeb (720)" "main(constellation) oneweb.tcl"} \
	{} \
	{"COMMstellation (78)" "main(constellation) commstellation-78.tcl"} \
	{"NeLS (120)" "main(constellation) nels.tcl"} \
	{"Celestri (63)" "main(constellation) celestri.tcl"} \
	{"SkyBridge (64)" "main(constellation) skybridge-64.tcl"} \
	{"SkyBridge (80)" "main(constellation) skybridge-80.tcl"} \
        {"Teledesic (288)" "main(constellation) teledesic-288.tcl"} \
        {"Teledesic (840)" "main(constellation) teledesic-840.tcl"} \
	{} \
	{"Aries" "main(constellation) aries.tcl"} \
	{"Boeing Higgins patent" "main(constellation) us-patent-6726152-boeing.tcl"} \
	{"Deligo" "main(constellation) deligo.tcl"} \
	{"@contact" "main(constellation) atcontact.tcl"} \
	{"Ellipso" "main(constellation) ellipso.tcl"} \
	{"GS2" "main(constellation) gs2.tcl"} \
	{"ICO" "main(constellation) ico.tcl"} \
	{"Macrocell" "main(constellation) macrocell.tcl"} \
	{"Millimeter elliptical" "main(constellation) mm.tcl"} \
	{"NUONCE" "main(constellation) nuonce.tcl"} \
	{"Odyssey" "main(constellation) odyssey.tcl"} \
	{"Spaceway NGSO" "main(constellation) spaceway-ngso.tcl"} \
	{} \
	{"Geosynchronous belt (TLE)" "main(constellation) geo.tle"} \
	{"Brightest LEO craft (TLE)" "main(constellation) visual.tle"}

}

proc main(rendering_menu) {submenu} {
    global geomview_module

    if {$geomview_module == 1} {
	build_options_menu $submenu \
	    {"Real-time mode" realtime_flag} \
	    {} \
	    {"Mark satellite positions" satellites_flag} \
	    {"Sphere markers" sphere_sat_flag} \
	    {"Box markers" box_sat_flag} \
	    {"Planes markers" planes_sat_flag} \
            {"Satellite markers" fancy_sat_flag} \
	    {} \
	    {"Show satellite orbits" orbits_flag} \
	    {"Show footprints" footprints_flag} \
	    {"Show coverage cones" cones_flag} \
	    {} \
	    {"Show stationary axes" axes_flag} \
	    {"Show equatorial plane" plane_flag} \
	    {"Show sunlight" sun_flag} \
	    {"Show star field" stars_flag} \
	    {} \
	    {"Show central body" earth_flag} \
	    {"Use simple Earth map" simple_earth_flag} \
	    {"Use detailed Earth map" fancy_earth_flag} \
	    {} \
    	    {"Animate in Geomview" geomview_flag}
    } else {
	build_options_menu $submenu \
	    {"Real-time mode" realtime_flag}
    }
}

proc main(forw_step) {} {

    main(stop)
    satellites FORW_STEP

}

proc main(back_step) {} {

    main(stop)
    satellites BACK_STEP

}


proc main(forwards) {} {
    global COLOR playbar

    satellites STOP
    if {("OK" == [satellites FORWARDS]) && ([winfo exists .main])} {
	.main.bb1.b3 configure -bg $COLOR(bg) -highlightbackground $COLOR(bg)
	.main.bb1.b4 configure -bg $COLOR(bg) -highlightbackground $COLOR(bg)
	.main.bb1.b5 configure -bg $COLOR(sbg) -highlightbackground $COLOR(sbg)
    }
    set flag $playbar
    set playbar 1
    if {$flag == 0} {
	coverage(forwards)
    }
    set playbar 0
}

proc main(backwards) {} {
    global COLOR playbar

    satellites STOP
    if {("OK" == [satellites BACKWARDS]) && ([winfo exists .main])} {
	.main.bb1.b3 configure -bg $COLOR(bg) -highlightbackground $COLOR(bg)
	.main.bb1.b4 configure -bg $COLOR(sbg) -highlightbackground $COLOR(sbg)
	.main.bb1.b5 configure -bg $COLOR(bg) -highlightbackground $COLOR(bg)
    }
    set flag $playbar
    set playbar 1
    if {$flag == 0} {
	coverage(backwards)
    }
    set playbar 0
}

proc main(stop) {} {
    global COLOR playbar

    if {("OK" == [satellites STOP]) && ([winfo exists .main])} {
	.main.bb1.b3 configure -bg $COLOR(sbg) -highlightbackground $COLOR(sbg)
	.main.bb1.b4 configure -bg $COLOR(bg) -highlightbackground $COLOR(bg)
	.main.bb1.b5 configure -bg $COLOR(bg) -highlightbackground $COLOR(bg)
    }

    set flag $playbar
    set playbar 1
    if {$flag == 0} {
	coverage(stop)
    }
    set playbar 0
}

proc main(restart) {} {

    main(stop)
    satellites RESET

}

proc main(exit) {} {
    global geomview_module fancy_earth_flag simple_earth_flag earth_flag

    if {$geomview_module == 1} {
	# turn off Earth entirely
	set fancy_earth_flag 0
	set simple_earth_flag 0
	set earth_flag 0
    }

    satellites EXIT

    puts stderr "\nSaVi: thankyou for using SaVi.\n"
    exit
}

proc main(update) {} {
    global sun_flag

    # 0 shows sunlight
    set i 0

    set n [.main.cmd.lb size]

    while {[satellites GET $i] != ""} {
        main(update_field) $i
	incr i
    }
    if {$n > 0} {
	.main.cmd.lb delete $i [expr $i+$n-1]
    }

    if {!$sun_flag} {
        .main.cmd.lb delete 0
    }

    edit(update)
    fisheye(update)
    coverage(update)
}

proc main(update_field) {i} {

    set noe [satellites GET $i]
    set name [satellites GET_NAME $i]
    set id [satellites GET_ID $i]

    # don't display everything in results string. Increase to 299 to debug.
    set length 72
    if {$id > 9} { incr length -1 }
    if {$id > 99} { incr length -1 }
    if {$id > 999} { incr length -1 }

    set line [list $id [string range $noe 0 $length] $name]

    # primitive, but works.
    regsub {\{} $line "" line
    regsub {\}} $line "" line
    regsub {\{} $line "" line
    regsub {\}} $line "" line

    .main.cmd.lb insert $i $line
}

proc main(show_sunlight) {} {
    main(update_field) 0
}

proc main(hide_sunlight) {} {
    .main.cmd.lb delete 0
}

proc main(update_one) {i} {
    global sun_flag

    main(update_field) $i
    .main.cmd.lb select set $i
    if {$sun_flag == 1} {
       incr i
    } else {
       incr i -1
    }
    .main.cmd.lb delete $i
}

proc main(update_selection) {m} {

    edit(set) $m

    # need to update so that selected satellites are marked purple.
    coverage(update)
    fisheye(update)
}

proc main(unselect) {n} {
    global sun_flag

    satellites GV_BEGIN
    foreach arg $n {
	set m [expr $arg + (1 - $sun_flag)]
	satellites UNTAG $m
    }
    satellites GV_END

    main(update)
}

proc main(unselect_all) {} {

    set n [.main.cmd.lb curselection]

    main(unselect) $n
}

proc main(tag_select) {n} {
    global sun_flag

    satellites GV_BEGIN
    foreach arg $n {
	set m [expr $arg + (1 - $sun_flag)]
	satellites TAG $m
    }
    satellites GV_END

    main(update_selection) $m
}

proc main(tag_unselect) {n} {
    global sun_flag

    satellites GV_BEGIN
    foreach arg $n {
	set m [expr $arg + (1 - $sun_flag)]
	satellites UNTAG $m
    }
    satellites GV_END

    main(update_selection) $m
}

proc main(untag_all) {} {

    set s [.main.cmd.lb size]

    satellites GV_BEGIN
    for {set m 0} {$m <= $s} {incr m} {
        satellites UNTAG $m
    }
    satellites GV_END
}

proc main(select) {n} {
    main(untag_all)
    main(tag_select) $n

}

proc main(dblselect) {n} {
    main(select) $n

    edit(build)
}

proc main(itemselect) {n} {
    set s [.main.cmd.lb curselection]

    set found [lsearch -exact $s $n]

    if {$found == -1} {
      .main.cmd.lb select set $n
      main(tag_select) $n
    } else {
      .main.cmd.lb select clear $n
      main(tag_unselect) $n
    }

}

proc main(itemdblselect) {n} {
    main(itemselect) $n
    edit(build)
}

proc main(rangeselect) {n} {
    global sun_flag

    set s [.main.cmd.lb curselection]
    set lowest 999999
    set highest 0

    foreach arg $s {
	if {$lowest > $arg} {
	    set lowest $arg
	}
	if {$highest < $arg} {
	    set highest $arg
        }
    }

    satellites GV_BEGIN
    if {($lowest > $n)&&($highest > $n)} {
	# tag down to n from lowest
	for {set i $n} {$i <= $lowest} {incr i} {
	    satellites TAG $i
	    .main.cmd.lb select set $i
	}
    } elseif {($highest < $n)&&($lowest < $n)} {
	# tag up to n from highest
	for {set i $highest} {$i <= $n} {incr i} {
	    satellites TAG $i
	    .main.cmd.lb select set $i
	}
    } else {
	# tag between lowest and highest, covering n
	for {set i $lowest} {$i <= $highest} {incr i} {
	    satellites TAG $i
	    .main.cmd.lb select set $i
	}
    }
    satellites GV_END

    main(update_selection) [expr $n + (1 - $sun_flag)]
}

proc main(rangedblselect) {n} {
    main(rangeselect) $n
    edit(build)
}

proc main(new) {} {
    set nn [satellites NEW]
    main(update)
    main(select) $nn
    edit(build)
}

proc main(copy) {} {
    global sun_flag

    set n [.main.cmd.lb curselection]

    foreach arg $n {
        set m [expr $arg + (1 - $sun_flag)]
	set nn [satellites COPY $m]
	main(select) $nn
    }
    main(update)
}

proc main(delete) {} {
    global sun_flag

    set n [.main.cmd.lb curselection]
    set i 0

    foreach arg $n {
	set m [expr $arg + (1 - $sun_flag) - $i]
	if {$sun_flag && ($m == 0)} {
	    puts stderr "\nSaVi: sunlight may be hidden, but not deleted."
	} else {
	    satellites DELETE $m
	    # deleting shortens list.
	    set i [expr $i + 1]
	}
    }
    main(update)
    coverage(update_decay)
}

proc main(delete_all) {} {
    global project_tracks_flag last_filename main_name details_source source_comments

    set project_tracks_flag 0
    satellites DELETE_ALL

    # restore sunlight at position 0.
    sunlight(create)

    set last_filename " "
    set details_source " "

    set source_comments " "
    details(rebuild)

    main(reset_title)

    geomview(update_texture)
    coverage(update_decay)
    main(update)
}


proc main(constellation) {filename} {
    global geomview_module last_filename PI params

    # We could choose not to delete all satellites when loading
    # in an additional constellation, but since mask angle is
    # set for all rather than for each satellite, the result
    # would be very confusing coverage. Load... gets around this.
    satellites DELETE_ALL

    # restore sunlight at position 0.
    sunlight(create)

    # coverage(update_decay)

    main(load_file) "data/$filename"
}

proc main(source_file) {filename} {
    global coverage_angle coverage_angle_flag PI params source_comments details_source
    global min_transmit_altitude

    set coverage_angle_old $coverage_angle
    set min_transmit_altitude_old $min_transmit_altitude

    # interpret as a tcl file. Set up constants.
    set MU $params(Mu)
    set RADIUS_OF_EARTH $params(Radius)

    source "$filename"
    set block_end 1

    set f [open "$filename" r]
    set source_comments ""
    while {[gets $f line] >= 0} {
	set line [string trim $line]
	if {[string index $line 0] == "\#"} {
	    set length [string length "$line"]
	    if {[string index $line 2] == "*"} {
		if {$length > 3} {
		    set line [string range "$line" 4 $length]
		    if {([string index $line 0] == " ")||($block_end == 1)} {
			set source_comments "$source_comments\n$line "
			set block_end 0
		    } else {
			set source_comments "$source_comments$line "
		    }
		} else {
		    set source_comments "$source_comments\n\n"
		}
	    } else {
		set block_end 1
	    }
	}
    }
    close $f
    set details_source "$filename"

    puts stderr "\nSaVi: loaded $filename"

    if {$min_transmit_altitude != $min_transmit_altitude_old} {
        puts stderr "      changed minimum transmit altitude from $min_transmit_altitude_old to $min_transmit_altitude km."
    }
    if {$coverage_angle != $coverage_angle_old} {
	puts stderr "      changed coverage angle from $coverage_angle_old to $coverage_angle degrees."
	if {$coverage_angle_flag} {
	    puts stderr "      Coverage angle probably describes mask elevation."
	}
    }

    geomview(update_texture)
    details(rebuild)
}

proc main(load_file) {filename} {
    global geomview_module last_filename no_access_flag


    if {![file exists "$filename"]} {
	puts stderr "\nSaVi: could not find $filename"
	return
    }

    if {$geomview_module == 1} {
        satellites GV_SEND "(ui-freeze on)"
    }
    # if filename ends in ".tle" then interpret as a two-line-element file
    if {[string_ends "$filename" ".tle"]} {
	set last_filename "$filename"
	# tle file
        # if sunlight is on should really turn it off as misleading.
	tle_file_input "$filename"
    } elseif {[string_ends "$filename" ".tcl"]} {
	set last_filename "$filename"
	main(source_file) "$filename"
    } else {
	puts stderr "\nSaVi: $filename is not a recognised .tle/.tcl satellites file."
	puts stderr "      No satellites loaded."
    }

    if {$geomview_module == 1} {
	geomview(update_texture)
        satellites GV_SEND "(ui-freeze off)"
    }

    main(title) "$filename"
    main(update)
}
