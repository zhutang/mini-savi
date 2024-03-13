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
# about.tcl
# $Id: about.tcl,v 1.35 2017/01/02 06:11:32 lloydwood Exp $

proc about(build) {} {
    global debug

    if {[eval window(raise) about]} return

    set name [build_Toplevel about]

    set package_names ""
    if {$debug > 0} {
        set package_names [eval package names]
    }

    wm protocol $name WM_DELETE_WINDOW about(dismiss)

    build_Title $name "about SaVi - satellite constellation visualization"

    build_DismissButtonbar $name dbbar about(dismiss) \
	{"Copyright details..." "helpfile(build) COPYRIGHT"}

    set cmd [build_CmdFrame $name cmd]

    set logo [canvas $cmd.logo -width 150 -height 75]
    image create photo graphic -file "maps/savi-logo.gif"
    $logo create image 75 0 -image graphic -anchor n
    pack $logo

    build_Label $cmd l1 "Satellite constellation simulation and Visualization"
    $cmd.l1.label configure -font *-times-medium-i-normal--*-140-*-*-*-*-*-* \
	-fg blue
    pack $cmd.l1 -anchor n -ipadx 15m -ipady 2m

    build_Label $cmd l2 \
"http://savi.sourceforge.net/
A user manual is in SaVi's manual/ directory.


Originally created at The Geometry Center
by Patrick Worfolk and Robert Thurman.

Enhanced and maintained by Lloyd Wood.
http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/

SaVi is free software. You may redistribute SaVi and/or modify
SaVi only under the terms that are given in the file COPYRIGHT,
which you should have received along with your copy of SaVi.

Copyright (c) 2001-2017 by Lloyd Wood.
Copyright (c) 1995-1997 by The Geometry Center
http://www.geom.uiuc.edu/~worfolk/SaVi/

[satellites VERSION]

SaVi is using Tcl/Tk [info patchlevel] and obeyed the launch command:
[satellites COMMAND_LINE]
$package_names
"

    pack $cmd.l2 -anchor n -ipadx 15m -ipady 2m

    pack $cmd -expand 1 -fill both
}

proc about(dismiss) {} {
    destroy .about
}
