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
# load_url_tle.tcl
#
# $Id: load_url_tle.tcl,v 1.16 2017/01/02 06:11:33 lloydwood Exp $

proc load_url_tle(init) {} {
    global url_choice load_url

    set url_choice 0
    set load_url "http://"
}

proc load_url_tle(build) {} {
    global url_choice load_url

    if {[eval window(raise) load_url_tle]} return

    set name [build_Toplevel load_url_tle]

    wm protocol $name WM_DELETE_WINDOW load_url_tle(dismiss)

    build_Title $name "Load constellation TLE set from web URL..."

	set cmd [build_CmdFrame $name cmd]

	set cmda [build_StdFrame $name cmd]

    build_IPopupMenu $cmda p0 "" \
	url_choice load_url_tle(choice) { \
              "Enter URL of TLE:" \
              "Iridium (Celestrak)" \
              "Globalstar (Celestrak)" \
              "Orbcomm (Celestrak)" \
              "O3b Networks (Celestrak)" \
              "GPS (Celestrak)" \
              "Glonass (Celestrak)" \
              "Galileo (Celestrak)" \
              "Molniya (Celestrak) " \
              "Cubesats (Celestrak)" \
              "Planet Labs (Planet Labs)" \
        }
	pack $cmda.p0 -side left

    build_LabelEntryColumns $cmda url_path \
        {lentry "" {load_url} }

    bind $cmda.url_path.c0.0 <Return> {load_url_tle(load) $load_url}

    pack $cmda -expand 1 -fill x

    build_Buttonbar $name dbbar \
	{"Load elsets from web" {load_url_tle(load) $load_url} } \
	{"Cancel" "load_url_tle(dismiss)"}

    update
}

proc load_url_tle(choice) {} {
    global url_choice load_url

    # sources on web for TLEs:
    # http://celestrak.com/NORAD/elements/
    # http://www.idb.com.au/joomla/index.php

    switch $url_choice {
        0 {
              set load_url "http://"
        }
        1 {
              set load_url "http://www.celestrak.com/NORAD/elements/iridium.txt"
        }
        2 {
              set load_url "http://www.celestrak.com/NORAD/elements/globalstar.txt"
        }
	3 {
              set load_url "http://www.celestrak.com/NORAD/elements/orbcomm.txt"
        }
	4 {
              set load_url "http://www.celestrak.com/NORAD/elements/other-comm.txt"
        }
        5 {
              set load_url "http://www.celestrak.com/NORAD/elements/gps-ops.txt"
        }
        6 {
              set load_url "http://www.celestrak.com/NORAD/elements/glo-ops.txt"
        }
        7 {
              set load_url "http://www.celestrak.com/NORAD/elements/galileo.txt"
        }
        8 {
              set load_url "http://www.celestrak.com/NORAD/elements/molniya.txt"
        }
        9 {
              set load_url "http://www.celestrak.com/NORAD/elements/cubesat.txt"
        }
        10 {
              set load_url "http://ephemerides.planet-labs.com/planet_mc.tle"
        }
    }
}

proc load_url_tle(load) {url} {

if {[catch {package require http}]} {
    puts stderr "\nSaVi: Unable to download TLEs. Tcl http package not found."
    return
}

# https support requires third-party tls package by Matt Newman
# see http://wiki.tcl.tk/1475 and http://wiki.tcl.tk/2630

    set filename [file tail $url]
    if {{string first "http://" $url} < 0} {
        puts stderr "\nSaVi: needs a valid http url"
    }

    set r [http::geturl $url -binary 1]

    # we append .tle because SaVi insists that tle files end .tle
    set localfile "/tmp/$filename.tle"
    set n 1
    while {[file exists $localfile]} {
        set localfile "/tmp/$filename-$n.tle"
        incr n
    }
   set fo [open $localfile w]
   fconfigure $fo -translation binary
   puts -nonewline $fo [http::data $r]
   close $fo

   ::http::cleanup $r
   puts stderr "\nSaVi: downloaded \"$url\" to \"$localfile\""

   puts stderr "\nSaVi: replacing any existing satellites so that time 0 is consistent."
   main(delete_all)

   main(load_file) $localfile

   load_url_tle(dismiss)
}

proc load_url_tle(dismiss) {} {

    destroy .load_url_tle
}

proc load_url_tle(cancel) {} {

    load_url_tle(dismiss)
}
