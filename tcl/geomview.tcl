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
# geomview.tcl
#
# $Id: geomview.tcl,v 1.32 2017/01/02 06:11:33 lloydwood Exp $

proc geomview(new_camera) {} {

  geomview(puts) "(new-camera Camera)"
}

proc geomview(camera_north_pole) {} {

    geomview(camera_position) 0 0 3
}

proc geomview(camera_south_pole) {} {

    geomview(camera_position) 0 0 -3
}


proc geomview(camera_position) {x y z} {

    geomview(begin)

    geomview(puts) "(geometry A { QUAD 0 0 0  0 0 0  0 0 0  0 0 0  })"
    geomview(puts) "(geometry B { QUAD 0 0 0  0 0 0  0 0 0  0 0 0  })"

    geomview(puts) "(transform A g0 g0 translate $x $y $z)"
    geomview(puts) "(position-at c0 A)"
    geomview(puts) "(position-at c0 A)"
    geomview(puts) "(look-toward B c0)"

    geomview(puts) "(delete A)"
    geomview(puts) "(delete B)"

    geomview(end)
}

proc geomview(choose_background_color) {} {
    global COLOR

    set color [tk_chooseColor -initialcolor $COLOR(geomview) -parent .main \
	    -title "SaVi: background color in Geomview"]

    if {($color == "") || ($color == $COLOR(geomview))} return

    set COLOR(geomview) $color

    geomview(set_background_color)
}

proc geomview(set_background_color) {} {
    global COLOR

    set value [string range $COLOR(geomview) 1 6]
    set GEOM [expr 0x$value]

    set GEOMB [expr $GEOM % 256]
    set GEOMG [expr ($GEOM - $GEOMB)/256 % 256]
    set GEOMR [expr ($GEOM - $GEOMB - $GEOMG * 256) / 65536]

    set geomb [expr $GEOMB / 255.0]
    set geomg [expr $GEOMG / 255.0]
    set geomr [expr $GEOMR / 255.0]

    geomview(puts) "(backcolor focus $geomr $geomg $geomb)"
}

proc geomview(wait) {} {
    satellites GV_WAIT

}

proc geomview(begin) {} {

    satellites GV_BEGIN
}

proc geomview(end) {} {

    satellites GV_END
}

proc geomview(puts) v {

    satellites GV_SEND $v
}

proc geomview(update_texture) {} {
    global geomview_module earth_flag

    if {($geomview_module == 1)} {
	geomview(begin)
        # recreate the Earth if it exists
	set flag $earth_flag
	set earth_flag 0
	if {$flag == 1} {
            set earth_flag 1
        }
	geomview(end)
    }
}

proc geomview(update_cones_footprints) {} {

    global geomview_module cones_flag earth_flag footprints_flag

    if {$geomview_module == 1} {
	geomview(begin)

        # recreate the Earth if it exists
        set flag $earth_flag
        set earth_flag 0
        if {$flag == 1} {
            set earth_flag 1
        }
	set flag $footprints_flag
	set footprints_flag 0
        if {$flag == 1} {
	    set footprints_flag 1
	}
	set flag $cones_flag
	set cones_flag 0
        if {$flag == 1} {
	    set cones_flag 1
        }

	geomview(end)
    }
}
