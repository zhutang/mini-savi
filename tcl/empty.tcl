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
# empty.tcl
#
# $Id: empty.tcl,v 1.19 2017/01/02 06:11:33 lloydwood Exp $

proc empty(build) {} {
    if {[eval window(raise) empty]} return

    set name [build_Toplevel empty]

    wm protocol $name WM_DELETE_WINDOW empty(dismiss)

    build_Title $name "empty space?"

    build_Buttonbar $name dbbar \
	{"Empty space of all satellites" empty(delete_all)} \
        {"Cancel" empty(dismiss)}

    update
}

proc empty(delete_all) {} {
    main(delete_all)
    empty(dismiss)
}

proc empty(dismiss) {} {
    destroy .empty
}
