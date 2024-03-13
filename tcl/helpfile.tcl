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
# textfile.tcl
# $Id: helpfile.tcl,v 1.22 2017/01/02 06:11:33 lloydwood Exp $

proc helpfile(build) {helpfile} {
    global helpfile_name helpfile_text

    if {[eval window(raise) helpfile]} {
	helpfile(load) $helpfile
	return
    }

    set name [build_Toplevel helpfile]

    wm protocol $name WM_DELETE_WINDOW helpfile(dismiss)

    build_Title $name "help"

    build_DismissButtonbar $name dbbar helpfile(dismiss) \
	{"Introduction" "helpfile(build) README"} \
	{"Bugs" "helpfile(build) BUGS"} \
	{"Texturemapping" "helpfile(build) README-COVERAGE-TEXTUREMAP"} \
	{"Copyright" "helpfile(build) COPYRIGHT"}

    set cmd [build_CmdFrame $name cmd]

    build_Label $cmd on "Help from $helpfile_name"
    set helpfile_name $cmd.on

    $helpfile_name.label configure -font *-times-medium-i-normal--*-140-*-*-*-*-*-* \
	-fg blue

    set helpfile_text [build_Scrollingtext $cmd text 60 25]

    $helpfile_text configure -font *-courier-i-normal--*-12-*-*-*-*-*-* \

    helpfile(load) $helpfile

    pack $cmd.text -side left -anchor w -ipadx 15m -ipady 2m

    pack $cmd -anchor w -expand 1 -fill both
}

proc helpfile(dismiss) {} {
    destroy .helpfile
}

proc helpfile(load) {helpfile} {
    global helpfile_name helpfile_text

    set helpfile_file $helpfile

    if {[string compare "" $helpfile] == 0} {
	# special case starting SaVi without a constellation loaded.
	set file_text "\nThis is a simple low-Earth-orbiting equatorial example satellite."
   } elseif {![file exists $helpfile]} {
	set file_text "\nThe $helpfile textfile is not available to SaVi.\n\n$helpfile may have been removed or moved. SaVi may have been packaged incorrectly by a third party.\n\nPlease install the full version of SaVi, which includes $helpfile, from: http://savi.sf.net/"
    } else {
	set f [open "$helpfile" r]
	set file_text ""
	while {[gets $f line] >= 0} {
	    if {[string index $line 0] != "$"} {
		set file_text "$file_text\n$line "
	    }
	}
	close $f
    }

    $helpfile_name.label configure -text "Help from $helpfile"
    $helpfile_text delete 1.0 end
    $helpfile_text insert end $file_text
}
