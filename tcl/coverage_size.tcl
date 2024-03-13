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
# coverage_size.tcl
#
# $Id: coverage_size.tcl,v 1.18 2017/01/02 06:11:33 lloydwood Exp $

proc coverage_size(build) {} {
    global NUM_COLORS NUM_COLORS_PREV IM_H

    if {[eval window(raise) coverage_size]} return

    set name [build_Toplevel coverage_size]

    wm protocol $name WM_DELETE_WINDOW coverage_size(dismiss)

    build_Title $name "Choose map settings..."

    if {$NUM_COLORS == 0} {

      if {$NUM_COLORS_PREV == 0} {
        coverage_size(default)
      } else {
	set NUM_COLORS $NUM_COLORS_PREV
      }

      set cmd [build_CmdFrame $name cmd]

      set cmda [build_StdFrame $name cmd]

      build_LabelEntryColumns $cmda colors \
	{ientry "Number of coverage shading levels (1-19)" {NUM_COLORS}}

      bind $cmda.colors.c0.0 <Return> coverage_size(number)
      bind $cmda.colors.c0.0 <Tab> coverage_size(number)

      pack $cmda -expand 1 -fill x

    }

    if {$IM_H == 0} {
      build_Buttonbar $name dbbar \
	{"Large (1024x512)" coverage_size(large)} \
        {"Small (600x300)" coverage_size(small)} \
	{"Cancel" "coverage_size(cancel)"}
    } else {
      build_Buttonbar $name dbbar \
	 {"Set color levels" coverage_size(set)} \
	 {"Cancel" "coverage_size(dismiss)"}
    }

    update
}

proc coverage_size(default) {} {
    global IM_H NUM_COLORS

    if {$IM_H > 400} {
	# We are large map, with room for a large key
	set NUM_COLORS 14
    } else {
        # traditional default value for SaVi
        set NUM_COLORS 4
    }
}

proc coverage_size(number) {} {
    global NUM_COLORS IM_H

    if {($NUM_COLORS < 1) || ($NUM_COLORS > 19)} {
        coverage_size(default)
	return
    }

    if {$IM_H == 0} return

    coverage_size(dismiss)
    coverage(build)
}

proc coverage_size(set) {} {

    coverage_size(number)

    coverage_size(dismiss)
    coverage(build)

}

proc coverage_size(large) {} {
    global IM_W IM_H

    coverage_size(number)

    set IM_W 1024
    set IM_H 512

    coverage_size(dismiss)
    coverage(build)
}

proc coverage_size(small) {} {
    global IM_W IM_H

    coverage_size(number)

    set IM_W 600
    set IM_H 300

    coverage_size(dismiss)
    coverage(build)
}


proc coverage_size(dismiss) {} {

    destroy .coverage_size
}

proc coverage_size(cancel) {} {
    global NUM_COLORS

    set NUM_COLORS 0
    coverage_size(dismiss)
}
