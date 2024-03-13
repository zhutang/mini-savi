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
# load.tcl
#
# $Id: load.tcl,v 1.22 2017/07/02 11:07:11 lloydwood Exp $

proc load(build) {} {

    set types {
        {"SaVi simulation scripts"  {.tcl} }
        {"Two-line element sets" {.tle} }
    }

    set filename [tk_getOpenFile -initialdir "data/" -filetypes $types \
	          -parent .main -title "SaVi: load satellites" ]

    if {"$filename" == ""} return

    main(load_file) "$filename"

}
