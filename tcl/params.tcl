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
# params.tcl
#
# $Id: params.tcl,v 1.18 2017/01/02 06:11:33 lloydwood Exp $


proc params(build) {} {
    if {[eval window(raise) params]} return

    set name [build_Toplevel params]

    wm protocol $name WM_DELETE_WINDOW params(dismiss)

    build_Title $name "Simulation constants"

    set cmd [build_StdFrame $name cmd]

    set cmda [build_StdFrame $cmd a]

    build_LabelEntryColumns $cmda le0 \
	    {text " " {"Radius (km)" "Rotation rate (rad/s)" } } \
	    {dentry "Central body" {tparams(Radius) tparams(Omega) } }

    pack $cmda -side left -expand 1

    set cmdb [build_StdFrame $cmd b]

    build_LabelEntryColumns $cmdb le1 \
	{text " " {"Mu (km^3/s^2)" "Oblateness (J2)"} } \
	{dentry "Gravitational model" {tparams(Mu) tparams(J2) } }

    pack $cmdb -side right -expand 1

    pack $cmd


    build_DismissButtonbar $name dbbar "params(dismiss)" \
	{"Reset to default" params(reset)} \
	{"Cancel" params(cancel)} \
	{"Apply" params(apply)}


    update

    # fill in the boxes
    params(cancel)
}

proc params(dismiss) {} {
    destroy .params
    params(cancel)
}

proc params(apply) {} {
    global params tparams

    set inds [array names tparams]
    foreach ind $inds {
	set params($ind) $tparams($ind)
    }

    # now update other panels if necessary
    edit(update)
}

proc params(cancel) {} {
    global params tparams

    set inds [array names tparams]
    foreach ind $inds {
	set tparams($ind) $params($ind)
    }
}

proc params(reset) {} {
    satellites RESET_PARAMS
    params(cancel)
}
