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
# star.tcl
#
# $Id: star.tcl,v 1.12 2017/01/02 06:11:33 lloydwood Exp $

proc star(build) {} {
    global star_oe coverage_angle

    set star_oe(name) "star"

    if {[eval window(raise) star]} return

    set name [build_Toplevel star]

    wm protocol $name WM_DELETE_WINDOW star(dismiss)

    build_Title $name "create crude star constellation"

    set cmd [build_CmdFrame $name cmd]
    build_LabelEntryColumns $cmd le0 \
	{text " " {"Star parameters" \
		         "Number of orbital planes" \
			 "Number of satellites per plane" \
		         "altitude (km)" \
			 "inclination (deg)" \
		         "eccentricity e" \
		         "omega" \
			 "long. asc. node offset (deg)" \
		         "overall phase offset (deg)" \
		         "coverage angle (deg)" } } \
	      {dentry "Orbital elements" {star_oe(P) star_oe(S) \
					      star_oe(alt) star_oe(inc) \
					      star_oe(e) star_oe(omega) \
					      star_oe(lan_offset) \
					      star_oe(phase_offset) \
					      coverage_angle } } \
	      {slider " " { star(slide_P) star(slide_S) \
				star(slide_alt) star(slide_inc) star(slide_e) \
			        star(slide_omega) star(slide_lan_offset) \
			        star(slide_phase_offset) star(coverage_angle)} }

    pack $cmd -anchor n -fill both -expand 1

    build_LabelEntryColumns $cmd lename \
        {lentry "Star name" star_oe(name)}

    for {set i 0} {$i < 9} {incr i} {
      bind $cmd.le0.c1.$i <Return> star(create)
      bind $cmd.le0.c1.$i <Tab> star(create)
    }

    build_Buttonbar $name dbbar \
	{"Create crude star" star(create)} \
	{"Dismiss" star(dismiss)}

    update

    star(reset)
}

proc star(dismiss) {} {
    destroy .star
}

proc star(update) {} {
    update
}

proc star(create) {} {
    global star_oe last_filename

    if {$star_oe(P) < 0} return
    if {$star_oe(S) < 0} return
    if {($star_oe(inc) < -90) || ($star_oe(inc) > 90)} return
    if {($star_oe(omega) < -180) || ($star_oe(omega) > 180)} return
    if {($star_oe(lan_offset) < -180) || ($star_oe(lan_offset) > 180)} return
    if {($star_oe(phase_offset) < -180) || ($star_oe(phase_offset) > 180)} return

    main(delete_all)
    walker_star $star_oe(P) $star_oe(S) \
	$star_oe(alt) $star_oe(inc) $star_oe(e) $star_oe(omega) \
	$star_oe(lan_offset) $star_oe(phase_offset) $star_oe(name)

    set last_filename "Crude star"

    main(update)
}

proc star(slide_P) v {
    global star_oe

    set star_oe(P) [expr $v+3]
}

proc star(slide_S) v {
    global star_oe

    set star_oe(S) [expr $v+1]
}

proc star(slide_F) v {
    global star_oe

    set star_oe(F) [expr $v]
}


proc star(slide_alt) v {
    global star_oe

    set star_oe(alt) [expr $v*300]
}

proc star(slide_inc) v {
    global star_oe

    set star_oe(inc) [expr $v*1.8 - 90]
}

proc star(slide_e) v {
    global star_oe

    # Don't want slider to allow eccentricity > 0.85
    # as this shows movement of orbit over time.
    set star_oe(e) [expr $v/125.0]
}

proc star(slide_omega) v {
    global star_oe

    set star_oe(omega) [expr $v*3.6 - 180]
}

proc star(slide_lan_offset) v {
    global star_oe

    set star_oe(lan_offset) [expr $v*3.6 - 180]
}

proc star(slide_phase_offset) v {
    global star_oe

    set star_oe(phase_offset) [expr $v*3.6 - 180]
}

proc star(coverage_angle) v {
    global coverage_angle

    set coverage_angle [expr $v*0.9]
}

proc star(iridium) {} {
    global star_oe coverage_angle

    set star_oe(name) "Crude-star"

    set star_oe(P) 6
    set star_oe(S) 11
    set star_oe(alt) 780
    set star_oe(inc) 86.4
    set star_oe(e) 0.0
    set star_oe(omega) 0.0
    set star_oe(lan_offset) 0.0
    set star_oe(phase_offset) 0.0

    set coverage_angle 8.2
}

proc star(reset) {} {
    # start with sensible values for a popular rosette - crude Iridium
    star(iridium)
}
