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
# oe.tcl
#
# Procedures for computing with orbital elements
#
# $Id: oe.tcl,v 1.16 2017/01/02 06:11:33 lloydwood Exp $

proc oe_to_period {a mu} {
    global PI

    set period [expr 2*$PI*pow($a,1.5)/sqrt($mu)]

}
