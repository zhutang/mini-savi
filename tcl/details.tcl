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
# details.tcl
# $Id: details.tcl,v 1.20 2017/01/02 06:11:33 lloydwood Exp $

proc details(build) {} {
    global last_filename source_comments comments_text comments_file details_source

    if {[eval window(raise) details]} return

    set name [build_Toplevel details]

    wm protocol $name WM_DELETE_WINDOW details(dismiss)

    build_Title $name "details of the loaded constellation"

    build_DismissButtonbar $name dbbar details(dismiss) \
	{"View source..." "helpfile(build) $details_source"} \
	{"Help..." "helpfile(build) README"}

    set cmd [build_CmdFrame $name cmd]

    build_Label $cmd on "Comments on $last_filename"
    set comments_file $cmd.on

    $comments_file.label configure -font *-times-medium-i-normal--*-140-*-*-*-*-*-* \
	-fg blue

    set comments_text [build_Scrollingtext $cmd text 50 10]

    $comments_text insert end "\nThis window describes details of the currently loaded constellation. Further details are in the constellation scripts in SaVi's data/ directory. This window can be closed, and later reopened from the Help menu.\n\n"
    $comments_text insert end $source_comments

    pack $cmd.text -side left -anchor w -ipadx 15m -ipady 2m

    pack $cmd -anchor w -expand 1 -fill both
}

proc details(dismiss) {} {
    destroy .details
}

proc details(rebuild) {} {
    global source_comments last_filename comments_text comments_file

    if {[eval window(raise) details]} {
	$comments_file.label configure -text "Comments on $last_filename"

        $comments_text delete 1.0 end
	$comments_text insert end $source_comments
    }
}
