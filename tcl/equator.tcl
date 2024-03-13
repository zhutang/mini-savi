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
# equator.tcl
#
# $Id: equator.tcl,v 1.7 2017/01/02 06:11:33 lloydwood Exp $

set previous_parallels_angle 0

proc equator(init) {} {
    global parallels_angle equatorial_exclusion_angle

    set equatorial_exclusion_angle 3.5

    set parallels_angle $equatorial_exclusion_angle
}

proc equator(create) {} {

    global PI params parallels_angle

    # lots of satellites to mark the geostationary or other rings.

    set MU $params(Mu)

    # markers every degree of longitude
    set NUM_PLANES 360

    # setup orbital elements
    # need to compute radius in gravitational field with MU instead!
    set a [expr 35786.1+$params(Radius)]
    set omega 0.0
    set T 0

    # not yet requiring geomview.
    satellites GV_BEGIN

    for {set i 0} {$i < $NUM_PLANES} {incr i} {
       # SaVi handles wraparound beyond +/-180
       set Omega [expr 360.0/$NUM_PLANES * $i]

       set name "equator-$i"
       satellites LOAD_MARKER $a $Omega $omega $T $name
    }

    set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]
    set T [expr $T_per * $parallels_angle/360]

    for {set i 0} {$i < $NUM_PLANES} {incr i} {

       set Omega [expr 360.0/$NUM_PLANES * $i]

       set name "parallel-N-$i"
       satellites LOAD_MARKER $a $Omega $omega $T $name
       set name "parallel-S-$i"
       satellites LOAD_MARKER $a $Omega $omega -$T $name
    }

    satellites GV_END
}

proc equator(update_angle) {} {

    global parallels_angle

    # local variable defined at top of this file
    upvar 1 previous_parallels_angle previous_parallels_angle

    set parallels_angle [expr abs($parallels_angle)]

    if {$parallels_angle > 90} {
	set parallels_angle 90
    }

    if {$parallels_angle == $previous_parallels_angle} {
	return
    }
    set previous_parallels_angle $parallels_angle
    
    equator(create)

    fisheye(update)

}
