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
# save.tcl
#
# $Id: save.tcl,v 1.31 2017/07/02 11:07:11 lloydwood Exp $

proc save(build) {} {
    global last_filename user_file_dir

    set types {
        {"SaVi simulation scripts"  {.tcl} }
    }

    set filenotfound 1

    while {$filenotfound == 1} {

	set filename [tk_getSaveFile -initialdir "$user_file_dir" -filetypes $types \
		      -title "SaVi: save satellites" ]

	if {"$filename" == ""} return

	set filename [save(extension) "$filename" "tcl"]

	# are we overwriting a file that adding the extension didn't catch?
	if {[file exists $filename]} {
	    puts stderr "\nSaVi: Not saved! $filename already exists!"
	} else {
	    set filenotfound 0
	}
    }

    # open file
    set f [open "$filename" w]
    if {$f == ""} {
	puts stderr "\nSaVi: couldn't save satellites to $filename"
	return
    }

    puts $f "\# SaVi saved satellites"
    puts $f "\# http://savi.sourceforge.net/"

    set n [.main.cmd.lb size]

    # put a wrapper around satellite creation
    puts $f "satellites GV_BEGIN"

    # write satellites
    # skip 0, as that's sunlight
    for {set i 1} {$i <= $n} {incr i} {
	set noe [satellites GET $i]
	puts -nonewline $f "satellites LOAD "
	puts $f $noe
    }

    puts $f "\# satellite names, if any, follow."
    for {set i 1} {$i <= $n} {incr i} {
	set noe [satellites GET_NAME $i]
	if {$noe != ""} {
	    puts $f "satellites NAME $i {$noe}"
	}
    }

    # close file
    puts $f "satellites GV_END"
    close $f

    puts stderr "\nSaVi: saved satellites to $filename"

    set last_filename "$filename"

    main(title) "$filename"
}

proc save(extension) {filename extension} {

    if {[string last "$extension" ".$filename"] < 0} {
	return "$filename.$extension"
    } else {
	return "$filename"
    }
}
