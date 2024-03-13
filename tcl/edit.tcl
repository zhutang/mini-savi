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
# edit.tcl
#
# $Id: edit.tcl,v 1.31 2017/01/02 06:11:33 lloydwood Exp $

proc edit(build) {} {
    global edit_oe geomview_module

    if {[eval window(raise) edit]} return

    set edit_oe(OK) 0

    set name [build_Toplevel edit]

    wm protocol $name WM_DELETE_WINDOW edit(dismiss)

    build_Title $name "Edit satellite parameters (press return to save)"

    set cmd [build_CmdFrame $name cmd]

    build_LabelEntryColumns $cmd lename \
	{lentry "Satellite name" edit_oe(name)}

    bind $cmd.lename.c0.0 <Return> edit(oe)
    bind $cmd.lename.c0.0 <Tab> edit(oe)

    build_LabelEntryColumns $cmd le0 \
	{text " " {"semi-major axis (km)" \
			 "eccentricity e" \
			 "inclination (deg)" \
			 "long. asc. node (deg)" \
			 "arg. of periapsis (deg)" \
			 "time to periapsis (s)"} } \
	      {dentry "Orbital elements" {edit_oe(a) edit_oe(e) edit_oe(i) \
					      edit_oe(Omega) edit_oe(omega) \
					      edit_oe(T)} } \
	      {slider " " { edit(slide_a) edit(slide_e) edit(slide_i) \
				edit(slide_O) edit(slide_o) edit(slide_T)} } \
        {ilabel {} {edit_oe(Period) edit_oe(Nodal_precession) \
                        edit_oe(Apsidal_rotation) edit_oe(Apoapsis_altitude) \
                    edit_oe(Periapsis_altitude)} } \
        {text {} {"period (minutes)" "nodal precession (deg/day)" "apsidal rotation (deg/day)" "apoapsis altitude (km)" "periapsis altitude (km)"} }

    for {set i 0} {$i < 6} {incr i} {
      bind $cmd.le0.c1.$i <Return> edit(oe)
      bind $cmd.le0.c1.$i <Tab> edit(oe)
    }

    if ($geomview_module) {
      build_HOptionslist $cmd le2 edit(oe) \
	{"Show satellite and groundtrack" edit_oe(disp_satellite)} \
	{"Show coverage area" edit_oe(disp_coverage)} \
	{"Show orbit" edit_oe(disp_orbit)}
    } else {
      build_HOptionslist $cmd le2 edit(oe) \
	{"Show satellite and groundtrack" edit_oe(disp_satellite)} \
	{"Show coverage area" edit_oe(disp_coverage)} \
    }

    pack $cmd -fill both -expand 1

    build_DismissButtonbar $name dbbar edit(dismiss)

    update
    set edit_oe(OK) 1
}

proc edit(dismiss) {} {
    destroy .edit
}

proc edit(oe) {} {
    global edit_oe sun_flag

    if {$edit_oe(n) < 0} {
	return
    }

    # could make edited satellite more visible by moving Geomview plane
    # to its plane of orbit
    # satellites PLANE $edit_oe(i) $edit_oe(Omega)
    # but this should really be set via an $edit_oe(disp_plane) option.

    set m $edit_oe(n)
    satellites NAME $m "$edit_oe(name)"
    satellites SET $m $edit_oe(a) $edit_oe(e) $edit_oe(i) \
	$edit_oe(Omega) $edit_oe(omega) $edit_oe(T) $edit_oe(disp_satellite) \
	$edit_oe(disp_orbit) $edit_oe(disp_coverage)
    edit(set) $m
    main(update_one) $m

    coverage(update)
    fisheye(update)
}

proc edit(slide_a) v {
    global edit_oe

    if {$edit_oe(OK) > 0} {
	set edit_oe(a) [expr $v*300+6400.0]
	edit(oe)
    }
}

proc edit(slide_e) v {
    global edit_oe

    if {$edit_oe(OK) > 0} {
	# Don't want slider to allow eccentricity > 0.85
	# as this shows movement of orbit over time.
	set edit_oe(e) [expr $v/125.0]
	edit(oe)
    }
}

proc edit(slide_i) v {
    global edit_oe

    if {$edit_oe(OK) > 0} {
	set edit_oe(i) [expr $v/100.0*180.0]
	edit(oe)
    }
}

proc edit(slide_O) v {
    global edit_oe

    if {$edit_oe(OK) > 0} {
	set edit_oe(Omega) [expr $v/100.0*360.0]
	edit(oe)
    }
}

proc edit(slide_o) v {
    global edit_oe

    if {$edit_oe(OK) > 0} {
	set edit_oe(omega) [expr $v/100.0*180.0]
	edit(oe)
    }
}

proc edit(slide_T) v {
    global edit_oe

    if {$edit_oe(OK) > 0} {
	set edit_oe(T) [expr $v/0.01]
	edit(oe)
    }
}

proc edit(update) {} {
    global sun_flag

    set n [.main.cmd.lb curselection]
    if {[llength $n] == 0} {
	edit(reset)
    } else {
	set m [expr $n + (1 - $sun_flag)]
	edit(set) [lindex $m 0]
	edit(oe)
    }
}


proc edit(reset) {} {
    global edit_oe

    set edit_oe(n) -1
    set edit_oe(a) ""
    set edit_oe(e) ""
    set edit_oe(i) ""
    set edit_oe(Omega) ""
    set edit_oe(omega) ""
    set edit_oe(T) ""
    set edit_oe(disp_satellite) 1
    set edit_oe(disp_orbit) 1
    set edit_oe(disp_coverage) 1

    set edit_oe(name) ""
}

proc edit(set) n {
    global edit_oe sun_flag
    set noe [satellites GET $n]
    if {$noe == ""} {
	edit(reset)
    } else {
	set edit_oe(n) $n
	set edit_oe(a) [lindex $noe 0]
	set edit_oe(e) [lindex $noe 1]
	set edit_oe(i) [lindex $noe 2]
	set edit_oe(Omega) [lindex $noe 3]
	set edit_oe(omega) [lindex $noe 4]
	set edit_oe(T) [lindex $noe 5]
	set edit_oe(Period) [lindex $noe 6]
	set edit_oe(Nodal_precession) [lindex $noe 7]
	set edit_oe(Apsidal_rotation) [lindex $noe 8]
	set edit_oe(Apoapsis_altitude) [lindex $noe 9]
	set edit_oe(Periapsis_altitude) [lindex $noe 10]
	set edit_oe(disp_satellite) [lindex $noe 11]
	set edit_oe(disp_orbit) [lindex $noe 12]
	set edit_oe(disp_coverage) [lindex $noe 13]

	set edit_oe(name) [eval satellites GET_NAME $n]
    }

}
