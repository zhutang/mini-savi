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
# no_frames.tcl
#
# $Id: no_frames.tcl,v 1.24 2017/07/02 11:07:11 lloydwood Exp $

proc no_frames(build) {} {
    if {[eval window(raise) no_frames]} return

    set name [build_Toplevel no_frames]

    wm protocol $name WM_DELETE_WINDOW no_frames(dismiss)

    build_Title $name "Choose no. of frames to save..."

    build_Buttonbar $name dbbar \
	{"10" "no_frames(save) 10"} \
	{"50" "no_frames(save) 50"} \
        {"100" "no_frames(save) 100"} \
        {"500" "no_frames(save) 500"} \
	{"1000" "no_frames(save) 1000"} \
        {"single snapshot" "no_frames(save) 1"} \
	{"Cancel" "no_frames(dismiss)"}

    update
}

proc no_frames(save) v {
    global user_file_dir

    if {$v < 0} return

    no_frames(dismiss)

    set types {
        {"SaVi snapshot frames"  {.ppm} }
    }

    set filename [tk_getSaveFile -initialdir "$user_file_dir" -filetypes $types \
	          -title "SaVi: save frames to folder" ]

    if {"$filename" == ""} return

    set length [string length "$filename"]

    if {[string_ends "$filename" ".ppm"]} {
        set filename [string range "$filename" 0 [expr $length - 5]]
    }

    if {$v == 1} {
	geomview(puts) "(snapshot focus $filename.ppm)"
    } else {
	for {set j 0} {$j < $v} {incr j} {
            set n  [format "%07d" $j]
	    geomview(wait)
	    geomview(puts) "(snapshot focus $filename$n.ppm)"
	    main(forw_step)
	    geomview(wait)
	}
    }
}

proc no_frames(dismiss) {} {

    destroy .no_frames
}
