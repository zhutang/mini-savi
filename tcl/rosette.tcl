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
# rosette.tcl
#
# $Id: rosette.tcl,v 1.25 2017/01/02 06:11:33 lloydwood Exp $

proc rosette(build) {} {
    global rosette_oe coverage_angle

    set rosette_oe(name) "rosette"

    if {[eval window(raise) rosette]} return

    set name [build_Toplevel rosette]

    wm protocol $name WM_DELETE_WINDOW rosette(dismiss)

    build_Title $name "create Ballard rosette (Walker delta)"

    set cmd [build_CmdFrame $name cmd]
    build_LabelEntryColumns $cmd le0 \
	{text " " {"Rosette parameters" \
		         "Number of orbital planes" \
			 "Number of satellites per plane" \
		         "Harmonic factor" \
		         "altitude (km)" \
			 "inclination (deg)" \
		         "eccentricity e" \
		         "omega" \
			 "long. asc. node offset (deg)" \
		         "overall phase offset (deg)" \
		         "coverage angle (deg)" } } \
	      {dentry "Orbital elements" {rosette_oe(P) rosette_oe(S) rosette_oe(F) \
					      rosette_oe(alt) rosette_oe(inc) \
					      rosette_oe(e) rosette_oe(omega) \
					      rosette_oe(lan_offset) \
					      rosette_oe(phase_offset) \
					      coverage_angle } } \
	      {slider " " { rosette(slide_P) rosette(slide_S) rosette(slide_F) \
				rosette(slide_alt) rosette(slide_inc) rosette(slide_e) \
			        rosette(slide_omega) rosette(slide_lan_offset) \
			        rosette(slide_phase_offset) rosette(coverage_angle)} }

    pack $cmd -anchor n -fill both -expand 1

    build_LabelEntryColumns $cmd lename \
        {lentry "Rosette name" rosette_oe(name)}

    for {set i 0} {$i < 10} {incr i} {
      bind $cmd.le0.c1.$i <Return> rosette(create)
      bind $cmd.le0.c1.$i <Tab> rosette(create)
    }

    build_Buttonbar $name dbbar \
	{"Create Ballard rosette" rosette(create)} \
	{"Celestri" rosette(celestri)} \
	{"ICO" rosette(ico)} \
	{"Galileo" rosette(galileo)} \
	{"Globalstar" rosette(globalstar)} \
	{"Dismiss" rosette(dismiss)}

    update

    rosette(reset)
}

proc rosette(dismiss) {} {
    destroy .rosette
}

proc rosette(update) {} {
    update
}

proc rosette(create) {} {
    global rosette_oe last_filename

    # range checks
    if {$rosette_oe(P) < 0} return
    if {$rosette_oe(S) < 0} return
    if {$rosette_oe(F) < 0} return
    if {($rosette_oe(inc) < -90) || ($rosette_oe(inc) > 90)} return
    if {($rosette_oe(omega) < -180) || ($rosette_oe(omega) > 180)} return
    if {($rosette_oe(lan_offset) < -180) || ($rosette_oe(lan_offset) > 180)} return
    if {($rosette_oe(phase_offset) < -180) || ($rosette_oe(phase_offset) > 180)} return

    if {$rosette_oe(F) > $rosette_oe(P)} {
        set prev $rosette_oe(F)
	set rosette_oe(F) [expr $rosette_oe(F)%$rosette_oe(P)]
        puts stderr "\nSaVi: Wrapping harmonic factor $prev to within number of planes."
	puts stderr "SaVi: Harmonic factor now $rosette_oe(F)"
    }

    main(delete_all)
    ballard_rosette $rosette_oe(P) $rosette_oe(S) $rosette_oe(F) \
	$rosette_oe(alt) $rosette_oe(inc) $rosette_oe(e) $rosette_oe(omega) \
	$rosette_oe(lan_offset) $rosette_oe(phase_offset) $rosette_oe(name)

    set last_filename "Ballard rosette"

    main(update)
}

proc rosette(slide_P) v {
    global rosette_oe

    set rosette_oe(P) [expr $v+3]
}

proc rosette(slide_S) v {
    global rosette_oe

    set rosette_oe(S) [expr $v+1]
}

proc rosette(slide_F) v {
    global rosette_oe

    set rosette_oe(F) [expr $v]
}


proc rosette(slide_alt) v {
    global rosette_oe

    set rosette_oe(alt) [expr $v*300]
}

proc rosette(slide_inc) v {
    global rosette_oe

    set rosette_oe(inc) [expr $v*1.8 - 90]
}

proc rosette(slide_e) v {
    global rosette_oe

    # Don't want slider to allow eccentricity > 0.85
    # as this shows movement of orbit over time.
    set rosette_oe(e) [expr $v/125.0]
}

proc rosette(slide_omega) v {
    global rosette_oe

    set rosette_oe(omega) [expr $v*3.6 - 180]
}

proc rosette(slide_lan_offset) v {
    global rosette_oe

    set rosette_oe(lan_offset) [expr $v*3.6 - 180]
}

proc rosette(slide_phase_offset) v {
    global rosette_oe

    set rosette_oe(phase_offset) [expr $v*3.6 - 180]
}

proc rosette(coverage_angle) v {
    global coverage_angle

    set coverage_angle [expr $v*0.9]
}

proc rosette(celestri) {} {
    global rosette_oe coverage_angle

    set rosette_oe(name) "Celestri-rosette"
    set rosette_oe(P) 7
    set rosette_oe(S) 9
    set rosette_oe(F) 5
    set rosette_oe(alt) 1400
    set rosette_oe(inc) 48.0
    set rosette_oe(e) 0.0013
    set rosette_oe(omega) 0.0
    set rosette_oe(lan_offset) 0.0
    set rosette_oe(phase_offset) 0.0

    set coverage_angle 16
}

proc rosette(galileo) {} {
    global rosette_oe coverage_angle

    set rosette_oe(name) "Galileo-rosette"

    set rosette_oe(P) 3
    set rosette_oe(S) 9
    set rosette_oe(F) 1
    set rosette_oe(alt) 23615.86
    set rosette_oe(inc) 56.0
    set rosette_oe(e) 0.0
    set rosette_oe(omega) 0.0
    set rosette_oe(lan_offset) 0.0
    set rosette_oe(phase_offset) 0.0

    set coverage_angle 0
}

proc rosette(globalstar) {} {
    global rosette_oe coverage_angle

    set rosette_oe(name) "Globlstar-rosette"

    set rosette_oe(P) 8
    set rosette_oe(S) 6
    set rosette_oe(F) 1
    set rosette_oe(alt) 1400
    set rosette_oe(inc) 52.0
    set rosette_oe(e) 0.0
    set rosette_oe(omega) 0.0
    set rosette_oe(lan_offset) 0.0
    set rosette_oe(phase_offset) 0.0

    set coverage_angle 10
}

proc rosette(ico) {} {
    global rosette_oe coverage_angle

    set rosette_oe(name) "ICO-rosette"

    set rosette_oe(P) 2
    set rosette_oe(S) 5
    set rosette_oe(F) 0
    set rosette_oe(alt) 10390
    set rosette_oe(inc) 45.0
    set rosette_oe(e) 0.0
    set rosette_oe(omega) 0.0
    set rosette_oe(lan_offset) 0.0
    set rosette_oe(phase_offset) 0.0

    set coverage_angle 10
}

proc rosette(reset) {} {
    # start with sensible values for a popular rosette - Globalstar
    rosette(globalstar)
}
