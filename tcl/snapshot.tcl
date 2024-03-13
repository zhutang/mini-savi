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
# large_snapshot.tcl
#
# $Id: snapshot.tcl,v 1.15 2017/07/02 11:07:11 lloydwood Exp $

proc snapshot(build) {} {
    global rendering_size

    if {[eval window(raise) snapshot]} return

    set name [build_Toplevel snapshot]

    wm protocol $name WM_DELETE_WINDOW snapshot(dismiss)

    build_Title $name "Choose size of snapshot to save..."

    set cmd [build_CmdFrame $name cmd]

    build_LabelEntryColumns $cmd x \
        {ientry "pixels across (x)" rendering_size(x)}
    build_LabelEntryColumns $cmd y \
        {ientry "pixels down (y)" rendering_size(y)}

    bind $cmd.x.c0 <Return> "snapshot(save)"
    bind $cmd.y.c0 <Tab> "snapshot(save)"

    pack $cmd -fill both -expand 1

    build_Buttonbar $name dbbar \
        {"Save snapshot" "snapshot(save)"} \
	{"Cancel" "snapshot(dismiss)"}

    update
}

proc snapshot(dismiss) {} {

    destroy .snapshot
}

proc snapshot(save) {} {
    global footprints_flag texture_flag geomview_dynamic_texture_flag
    global fast_marker_sat_flag sphere_sat_flag rendering_size user_file_dir

    snapshot(dismiss)

    set types {
        {"SaVi high-quality rendering" {.ppm} }
    }

    set filename [tk_getSaveFile -initialdir "$user_file_dir" -filetypes $types \
	          -title "SaVi: save rendering to folder" ]

    if {"$filename" == ""} return

    set filename [save(extension) "$filename" ppm]

    set footprints_on 0
    set texture_off 0
    set marker_off 0

    geomview(begin)

    # texturemapping doesn't appear in software snapshot,
    # so coverage footprints substitute.
    if {($texture_flag == 1) && ($geomview_dynamic_texture_flag == 1)} {
	set texture_flag 0
	set texture_off 0
	if {$footprints_flag == 0} {
	    set footprints_flag 1
	    set footprints_on 1
	}
    }

    # marker blobs are too small; spheres scales.
    if {$fast_marker_sat_flag == 1} {
	set fast_marker_sat_flag 0
	set marker_off 1
    }

    # change to white for print
    geomview(puts) "(backcolor focus 1 1 1)"

    geomview(puts) "(snapshot focus \"$filename\" ppm $rendering_size(x) $rendering_size(y))"

    # change back to current color
    geomview(set_background_color)

    if {$marker_off == 1} {
	set fast_marker_sat_flag 1
    }
    if {$footprints_on == 1} {
	set footprints_flag 0
    }
    if {$texture_off == 1} {
	set texture_flag 1
    }

    geomview(end)
}
