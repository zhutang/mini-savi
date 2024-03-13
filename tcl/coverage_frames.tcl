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
# coverage_frames.tcl
#
# $Id: coverage_frames.tcl,v 1.10 2017/07/02 11:07:11 lloydwood Exp $

proc coverage_frames(build) {} {
    global total_frames

    if {[eval window(raise) coverage_frames]} return

    set name [build_Toplevel coverage_frames]

    wm protocol $name WM_DELETE_WINDOW coverage_frames(dismiss)

    build_Title $name "Choose no. of frames to save..."

    build_LabelEntryColumns $name total_frames \
        {ientry "" {total_frames} } \
	{text "" {frames} }

    bind $name.total_frames.c0.0 <Return> {coverage_frames(save) $total_frames}

    build_Buttonbar $name dbbar \
	{"10" "coverage_frames(save) 10"} \
	{"50" "coverage_frames(save) 50"} \
        {"100" "coverage_frames(save) 100"} \
        {"500" "coverage_frames(save) 500"} \
	{"Cancel" "coverage_frames(dismiss)"} \
        {"Save" "coverage_frames(save) $total_frames"}

    update
}

proc coverage_frames(save) v {
    global im IM_H IM_W
    global user_file_dir

    if {$v < 0} return

    coverage_frames(dismiss)

    set types {
        {"SaVi coverage animation"  {.gif} }
    }

    set filename [tk_getSaveFile -initialdir "$user_file_dir" -filetypes $types \
	          -title "SaVi: save frames to folder" ]

    if {"$filename" == ""} return

    if {$v == 1} {
        # write a single image.
        $im write "$user_file_dir$filename" -format gif -from 0 0 $IM_W $IM_H
    } else {
        set gifsicle "gifsicle"
        set files "exec $gifsicle -O3"
        set rmfiles "exec rm"
	for {set j 0} {$j < $v} {incr j} {
            set n  [format "%07d" $j]
            set files "$files $filename-$n"
            set rmfiles "$rmfiles $filename-$n"
            $im write "$filename-$n" -format gif -from 0 0 $IM_W $IM_H
            main(forw_step)
	}
        if {[catch {eval $files > $filename}]} {
           puts stderr "\nSaVi: could not run $gifsicle successfully. Is $gifsicle installed?"
        } else {
           puts stderr "\nSaVi: told $gifsicle to save animation of $v frames in $filename"
        }
        eval $rmfiles
    }
}

proc coverage_frames(dismiss) {} {

    destroy .coverage_frames
}
