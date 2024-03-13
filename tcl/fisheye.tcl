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
# fisheye.tcl
#
# $Id: fisheye.tcl,v 1.71 2017/07/02 11:07:11 lloydwood Exp $

proc fisheye(init) {} {
    global fisheye_flag fisheye_reverse_view_flag fisheye_names_flag fisheye_numbers_flag \
	fisheye_times_flag fisheye_viewpoint_flag fisheye_print_inview_flag fisheye_labels_flag Fisheye \
        fisheye_flag_dismissed

    trace variable fisheye_flag w flag_change
    trace variable fisheye_labels_flag w flag_change
    trace variable fisheye_reverse_view_flag w flag_change
    trace variable fisheye_print_inview_flag w flag_change
    trace variable fisheye_names_flag w flag_change
    trace variable fisheye_numbers_flag w flag_change
    trace variable fisheye_times_flag w flag_change
    trace variable fisheye_viewpoint_flag w flag_change

    set fisheye_flag 0
    set fisheye_labels_flag 1
    set fisheye_reverse_view_flag 0
    set fisheye_print_inview_flag 0
    set fisheye_names_flag 1
    set fisheye_numbers_flag 0
    set fisheye_times_flag 0

    set fisheye_flag_dismissed 1

    # Equator (0,0) - increases chances of seeing a satellite
    set Fisheye(new_lat_degrees) 0
    set Fisheye(new_lat_minutes) 0
    set Fisheye(new_long_degrees) 0
    set Fisheye(new_long_minutes) 0
    set Fisheye(new_location_name) "Equator (0,0)"
    fisheye(set_location) $Fisheye(new_lat_degrees) $Fisheye(new_lat_minutes) $Fisheye(new_long_degrees) $Fisheye(new_long_minutes) $Fisheye(new_location_name)
}

proc fisheye(build) {} {
    global Fisheye fisheye_flag fisheye_flag_dismissed fisheye_names_flag
    global fisheye_times_flag fisheye_numbers_flag fisheye_viewpoint_flag
    global plane_flag parallels_angle

    if {[eval window(raise) fisheye]} return

    set fisheye_name [build_Toplevel fisheye]

    wm protocol $fisheye_name WM_DELETE_WINDOW fisheye(dismiss)

    build_Title $fisheye_name "Fisheye view of the local sky"

    build_Menubar $fisheye_name mbar \
	{"Help" \
	     {"About this constellation..." "details(build)" "H"} \
	     {} \
	     {"Introduction to SaVi..." "helpfile(build) README"} \
	     {"Bugs in SaVi..." "helpfile(build) BUGS"} \
	     {"Texturemapping in Geomview..." "helpfile(build) README-COVERAGE-TEXTUREMAP"} \
	     {} \
             {"about SaVi..." "about(build)"} } \
	{"File" \
	     {"Load satellites..." "load(build)"} \
	     {"Save satellites as Tcl script..." "save(build)" "S"} \
	     {} \
             {"Load new constellation TLEs from web..." "load_url_tle(build)"} \
             {} \
	     {"Save fisheye as postscript..." fisheye(save)} \
	     {} \
	     {"Close window" "fisheye(dismiss)" "W"} \
	     {"Quit" "main(exit)" "Q"} } \
	{"Edit" \
	     {"Simulation constants..." "params(build)"} } \
	{"Views" \
	     {"Satellites list..." "window(raise) main" "L"} \
	     {"Global coverage..." "coverage(build)" "G"} \
	     {} \
	     {"> Step forwards" "main(forw_step)"} \
	     {"< Step back" "main(back_step)"} \
	     {">> Forwards..." "main(forwards)"} \
	     {"<< Reverse..." "main(backwards)"} \
	     {"Freeze views" "main(stop)"} \
	     {} \
	     {"Restart views at time zero" "main(restart)" "0"} } \
	{"Constellations"} \
	{"Rendering"} \
	{"Display"} \
	{"Locations" \
	     {"Equator (0,0)" "fisheye(set_location) 0 0 0 0 {Equator (0 , 0)}"} \
	     {"North Pole" "fisheye(set_location) 90 0 0 0 {North Pole}"} \
	     {"South Pole" "fisheye(set_location) -90 0 0 0 {South Pole} "} \
	     {} \
	     {"Adelaide, Australia" "fisheye(set_location) -34 55 138 36 {Adelaide, Australia}"} \
	     {"Algiers, Algeria" "fisheye(set_location) 36 50 3 0 {Algiers, Algeria}"} \
	     {"Amsterdam, Netherlands" "fisheye(set_location) 52 22 4 53 {Amsterdam, Netherlands}"} \
	     {"Anchorage, Alaska" "fisheye(set_location) 61 13 -149 54 {Anchorage, Alaska}"} \
	     {"Ankara, Turkey" "fisheye(set_location) 39 55 32 55 {Ankara, Turkey}"} \
	     {"Athens, Greece" "fisheye(set_location) 37 58 23 43 {Athens, Greece}"} \
	     {"Auckland, New Zealand" "fisheye(set_location) -36 52 174 45 {Auckland, New Zealand}"} \
	     {"Bangkok, Thailand" "fisheye(set_location) 13 45 100 30 {Bangkok, Thailand}"} \
	     {"Beijing, China" "fisheye(set_location) 39 55 116 25 {Beijing, China}"} \
	     {"Berlin, Germany" "fisheye(set_location) 52 30 13 25 {Berlin, Germany}"} \
	     {"Bombay, India" "fisheye(set_location) 19 0 72 48 {Bombay, India}"} \
             {"Brisbane, Australia" "fisheye(set_location) -27 28 153 0 {Brisbane, Australia}"} \
	     {"Brussels, Belgium" "fisheye(set_location) 50 52 4 22 {Brussels, Belgium}"} \
	     {"Buenos Aires, Argentina" "fisheye(set_location) -34 35 -58 22 {Buenos Aires, Argentina}"} \
	     {"Canberra, Australia" "fisheye(set_location) -35 18 149 7 {Canberra, Australia}"} \
	     {"Copenhagen, Denmark" "fisheye(set_location) 55 40 12 34 {Copenhagen, Denmark}"} \
	     {"Dublin, Eire" "fisheye(set_location) 53 20 -6 15 {Dublin, Eire}"} \
	     {"Lima, Peru" "fisheye(set_location) -12 0 -77 2 {Lima, Peru}"} \
	     {"Lisbon, Portugal" "fisheye(set_location) 38 44 -9 9 {Lisbon, Portugal}"} \
	     {"London, UK" "fisheye(set_location) 51 32 0 -5 {London, UK}"} \
	     {"Madrid, Spain" "fisheye(set_location) 40 26 -3 42 {Madrid, Spain}"} \
	     {"Mecca, Saudi Arabia" "fisheye(set_location) 21 29 39 45 {Mecca, Saudi Arabia}"} \
	     {"Melbourne, Australia" "fisheye(set_location) -37 47 144 58 {Melbourne, Australia}"} \
             {"Miami, Florida" "fisheye(set_location) 25 47 -80 13 {Miami, Florida}"} \
	     {"Moscow, Russia" "fisheye(set_location) 55 45 37 36 {Moscow, Russia}"} \
	     {"New York, USA" "fisheye(set_location) 40 47 -73 58 {New York, USA}"} \
	     {"Oslo, Norway" "fisheye(set_location) 59 57 10 42 {Oslo, Norway}"} \
	     {"Paris, France" "fisheye(set_location) 48 48 2 20 {Paris, France}"} \
	     {"Perth, Australia" "fisheye(set_location) -31 57 115 51 {Perth, Australia}"} \
	     {"Port Moresby, Papua New Guinea" "fisheye(set_location) -9 25 147 17 {Port Moresby, Papua New Guinea}"} \
	     {"Quito, Ecuador" "fisheye(set_location) 0 -14 -78 30 {Quito, Ecuador}"} \
	     {"San Francisco, USA" "fisheye(set_location) 37 47 -122 26 {San Francisco, USA}"} \
	     {"Reykjavik, Iceland" "fisheye(set_location) 64 4 -21 58 {Reykjavik, Iceland}"} \
	     {"Rio de Janeiro, Brazil" "fisheye(set_location) -22 57 -43 12 {Rio de Janeiro, Brazil}"} \
	     {"Rome, Italy" "fisheye(set_location) 41 54 12 27 {Rome, Italy}"} \
	     {"Singapore" "fisheye(set_location) 1 14 103 55 {Singapore}"} \
	     {"Sydney, Australia" "fisheye(set_location) -34 0 151 0 {Sydney, Australia}"} \
	     {"Toronto, Canada" "fisheye(set_location) 43 40 -79 24 {Toronto, Canada}"} \
	     {"Tokyo, Japan" "fisheye(set_location) 35 40 139 45 {Tokyo, Japan}"} \
	     {"Vancouver, Canada" "fisheye(set_location) 49 13 -123 6 {Vancouver, Canada}"} \
	     {"Vienna, Austria" "fisheye(set_location) 48 14 16 20 {Vienna, Austria}"} \
	     {"Washington DC, USA" "fisheye(set_location) 38 53 -77 02 {Washington DC, USA}"} \
	     {"Wellington, New Zealand" "fisheye(set_location) -41 17 174 47 {Wellington, New Zealand}"} }

    main(constellations_menu) $fisheye_name.mbar.b3
    main(rendering_menu) $fisheye_name.mbar.b4

    build_options_menu $fisheye_name.mbar.b5 \
	{"Plot location on map" fisheye_viewpoint_flag } \
	{} \
	{"Show numbers" fisheye_numbers_flag} \
	{"Show names" fisheye_names_flag} \
	{"Show time in view" fisheye_times_flag} \
	{} \
 	{"Show stationary arc" plane_flag} \
	{} \
	{"Reverse fisheye view" fisheye_reverse_view_flag}

    set cmd [build_CmdFrame $fisheye_name cmd]

    set p "$cmd.p"
    canvas $p -bg black -height 360 -width 360

    bind $p <Button-2> {main(back_step)}
    bind $p <Button-3> {main(forw_step)}
    bind $p <Shift-Button-2> {main(backwards)}
    bind $p <Shift-Button-3> {main(forwards)}

    pack $p -side right
    pack $p -fill both -expand 1

    set cmdr [build_StdFrame $cmd r]

    build_LabelEntryColumns $cmdr le1 \
	{label "" {time Fisheye(location_name) fisheye_lat_lon}}

    pack $cmdr

    build_Optionslist $cmd olist \
	{"Plot location on map" fisheye_viewpoint_flag} \
	{"Numbers" fisheye_numbers_flag} \
	{"Names" fisheye_names_flag} \
	{"Time in view" fisheye_times_flag} \
	{"Stationary arc" plane_flag}

    set cmdb [build_StdFrame $cmd b]

    build_LabelEntryColumns $cmdb le2 \
        {text "" "Parallels"} \
        {dentry "" {parallels_angle}} \
	{text "" "degrees"}


    build_LabelEntryColumns $cmdb le3 \
        {text "" "Coverage"} \
        {dentry "" {coverage_angle}} \
	{text "" "degrees"}

    bind $cmdb.le2.c1.0 <Return> equator(update_angle)
    bind $cmdb.le2.c1.0 <Tab> equator(update_angle)
    bind $cmdb.le3.c1.0 <Return> coverage(update_angle)
    bind $cmdb.le3.c1.0 <Tab> coverage(update_angle)

    pack $cmdb -fill both -expand 1

    set cmdq [build_StdFrame $cmd q]

    build_LabelEntryColumns $cmdq le0 \
	{text " " {"Lat:" "Long:"}} \
	      {ientry "degrees" {Fisheye(new_lat_degrees) Fisheye(new_long_degrees)} } \
	{ientry "minutes" {Fisheye(new_lat_minutes) Fisheye(new_long_minutes)} }

    bind $cmdq.le0.c1.0 <Return> fisheye(apply)
    bind $cmdq.le0.c1.1 <Return> fisheye(apply)
    bind $cmdq.le0.c2.0 <Return> fisheye(apply)
    bind $cmdq.le0.c2.1 <Return> fisheye(apply)

    pack $cmdq -side bottom -fill x

    pack $cmd -fill both -expand 1


    # an explicit Dismiss Window button halts computation.
    build_DismissButtonbar $fisheye_name dbbar fisheye(dismiss) \
	{"Set new location" fisheye(apply)}
    update

    # needed if window was closed and is being reopened
    fisheye(draw)

    fisheye(update)
}

proc fisheye(dismiss) {} {
    global fisheye_flag fisheye_flag_dismissed

    set fisheye_flag_dismissed $fisheye_flag
    set fisheye_flag 0
    destroy .fisheye
}

proc fisheye(draw) {} {
    global FS_D coverage_angle

    # initialize display

    set p ".fisheye.cmd.p"
    if {[eval winfo exists $p] == 0} return

    set adjust 2

    set d [expr $FS_D - $adjust]

    set x_start [expr double($adjust)]
    set y_end [expr double($d - $x_start)]
    $p create oval $x_start $x_start $y_end $y_end -fill white -width 1 -tags disk

    # 30 degrees circle
    set x_start [expr $d / 6 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline black -width 1 -tags disk

    # 60 degrees circle
    set x_start [expr $d / 3 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline black -width 1 -tags disk


    # centre point
    set r [expr $d / 2]
    set r1 [expr $r + 1]
    $p create rectangle $r $r $r1 $r1 -outline black -width 1 -tags disk

    # if canvas is too small, don't draw extra circles.
    if {$d < 175} return;

    # 15 degrees circle
    set x_start [expr $d / 12 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline grey50 -width 1 -tags disk

    # 45 degrees circle
    set x_start [expr $d / 4 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline grey50 -width 1 -tags disk

    # 75 degrees circle
    set x_start [expr $d * 5/12 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline grey50 -width 1 -tags disk

    # draw more circles on big canvas.
    if {$d < 350} return;

    # 5 degrees circle
    set x_start [expr $d / 36 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline grey90 -width 1 -tags disk

    # 10 degrees circle
    set x_start [expr $d / 18 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline grey90 -width 1 -tags disk

    # 20 degrees circle
    set x_start [expr $d / 9 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline grey90 -width 1 -tags disk

    # 25 degrees circle
    set x_start [expr $d / 7.2 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline grey90 -width 1 -tags disk

    # 35 degrees circle
    set x_start [expr $d / 5.1429 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline grey90 -width 1 -tags disk

    # 40 degrees circle
    set x_start [expr $d / 4.5 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline grey90 -width 1 -tags disk

    # 50 degrees circle
    set x_start [expr $d / 3.6 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline grey90 -width 1 -tags disk

    # 55 degrees circle
    set x_start [expr $d / 3.273 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline grey90 -width 1 -tags disk

    # 65 degrees circle
    set x_start [expr $d / 2.77 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline grey90 -width 1 -tags disk

    # 70 degrees circle
    set x_start [expr $d / 2.57 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline grey90 -width 1 -tags disk

    # 80 degrees circle
    set x_start [expr $d / 2.25 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline grey90 -width 1 -tags disk

    # 85 degrees circle
    set x_start [expr $d / 2.118 + $adjust]
    set y_end [expr $d - $x_start]
    $p create oval $x_start $x_start $y_end $y_end -outline grey90 -width 1 -tags disk
}

proc fisheye(delete) {} {
    set p ".fisheye.cmd.p"

    if [winfo exists $p] {
	$p delete crosses
	$p delete labels
	$p delete mask_elevation
	$p delete equator
    }
}

proc fisheye(resize) {} {

    # update C with current smallest dimension of window.

    set p ".fisheye.cmd.p"
    set diam [eval winfo width $p]
    set height [eval winfo height $p]
    if {$diam > $height} {
        set diam $height
    }
    upvar #0 FS_D FS_D
    if {$diam != $FS_D} {
	$p delete disk
	set FS_D $diam
	fisheye(draw)
    }
}

proc fisheye(update) {} {
    global fisheye_flag

    if {![winfo exists .fisheye]} return

    set flag fisheye_flag
    set fisheye_flag 1
    if {$flag == 0} {
	set fisheye_flag 0
    }
    update
}



proc fisheye(apply) {} {
    global Fisheye

    if {($Fisheye(new_lat_degrees) == $Fisheye(lat_degrees)) &&
	($Fisheye(new_lat_minutes) == $Fisheye(lat_minutes)) &&
	($Fisheye(new_long_degrees) == $Fisheye(long_degrees)) &&
	($Fisheye(new_long_minutes) == $Fisheye(long_minutes))} {
	# do not change the name
	return
    }

    set Fisheye(new_location_name) "custom location"

    fisheye(set_location) $Fisheye(new_lat_degrees) $Fisheye(new_lat_minutes) $Fisheye(new_long_degrees) $Fisheye(new_long_minutes) $Fisheye(new_location_name)
}

proc fisheye(set_location) {lat_degrees lat_minutes long_degrees long_minutes location} {
    global Fisheye

    set Fisheye(lat_degrees) $lat_degrees
    set Fisheye(lat_minutes) $lat_minutes
    set Fisheye(long_degrees) $long_degrees
    set Fisheye(long_minutes) $long_minutes
    set Fisheye(location_name) $location
    satellites FISHEYE_SET $Fisheye(lat_degrees) $Fisheye(lat_minutes) \
	$Fisheye(long_degrees) $Fisheye(long_minutes)
    fisheye(update)
}

proc fisheye(save) {} {
    global user_file_dir

    set types {
        {"Postscript files"  {.ps} }
    }

    set filename [tk_getSaveFile -initialdir "$user_file_dir" -filetypes $types \
	          -title "SaVi: save fisheye as Postscript" ]

    if {"$filename" == ""} return

    set length [string length "$filename"]
    set last3 [string range "$filename" [expr $length - 3] $length]

    if {[string compare $last3 ".ps"] != 0} {
	set filename "$filename.ps"
    }

    # open file
    set f [open "$filename" w]
    if {$f == ""} {
	puts stderr "\nSaVi: couldn't save postscript to $filename"
	return
    }

    set p ".fisheye.cmd.p"
    $p postscript -channel $f

    # close file
    close $f

    puts stderr "\nSaVi: saved fisheye as postscript to $filename"
}
