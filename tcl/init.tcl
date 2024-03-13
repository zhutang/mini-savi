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
# init.tcl
#
# $Id: init.tcl,v 1.96 2017/07/02 11:07:11 lloydwood Exp $

proc geomview_init {} {
    global satellites_flag box_sat_flag planes_sat_flag fancy_sat_flag \
	   sphere_sat_flag fast_marker_sat_flag cones_flag footprints_flag \
	   orbits_flag axes_flag stars_flag \
	   earth_flag simple_earth_flag fancy_earth_flag texture_flag \
	   geomview_flag geomview_dynamic_texture_flag COLOR

    satellites GV_BEGIN

    set COLOR(geomview) \#000000

    trace variable satellites_flag w flag_change
    trace variable box_sat_flag w flag_change
    trace variable planes_sat_flag w flag_change
    trace variable fancy_sat_flag w flag_change
    trace variable sphere_sat_flag w flag_change
    trace variable fast_marker_sat_flag w flag_change

    trace variable orbits_flag w flag_change
    trace variable footprints_flag w flag_change
    trace variable cones_flag w flag_change

    trace variable axes_flag w flag_change
    trace variable stars_flag w flag_change

    trace variable earth_flag w flag_change
    trace variable simple_earth_flag w flag_change
    trace variable fancy_earth_flag w flag_change
    trace variable texture_flag w flag_change
    trace variable geomview_dynamic_texture_flag w flag_change


    trace variable geomview_flag w flag_change

    set satellites_flag 1
    set fast_marker_sat_flag 1
    set box_sat_flag 0
    set planes_sat_flag 0
    set fancy_sat_flag 0
    set sphere_sat_flag 0

    set orbits_flag 1
    set footprints_flag 0
    set cones_flag 0

    set axes_flag 1
    set stars_flag 0

    set earth_flag 1
    set simple_earth_flag 1
    set fancy_earth_flag 0
    set texture_flag 1

    set geomview_dynamic_texture_flag 0
    set geomview_flag 1

    satellites GV_END
}

proc app_init {} {
    global TITLE FONT COLOR NUM_COLORS NUM_COLORS_PREV PLATFORM Projection
    global first_filename last_filename source_comments comments_text comments_file details_source
    global user_file_dir helpfile_text helpfile_name geomview_module
    global realtime_flag sun_flag playbar delta_t splash_about rendering_size
    global plane_flag
    global modifier modifier_indicator

    # env is unused.

    global PI
    set PI 3.14159265358979324

    # delay per simulation interval
    set delta_t 60.0

    set playbar 0

    # set colors for windows

    if {[string equal [tk windowingsystem] aqua]} {
      # Aqua Tcl on Mac OS X - white
      # can't control menu highlights or button colors as you'd expect.
      # instead, -highlightbackground is needed to color the frame
      # around the edges and corners of the button.
      set COLOR(bg) #fff
      set COLOR(abg) #67f
      set COLOR(sbg) #33f
      set modifier "Command"
      set modifier_indicator "Command"
      set PLATFORM 1
    } elseif {[string equal [tk windowingsystem] win32]||[file exists "/proc/cygdrive"]} {
      # Tcl on Windows - grey to match XP. Vary to suit interface.
      set COLOR(bg) #d4d0c8
      set COLOR(abg) #bbb
      set COLOR(sbg) #aaa
      set modifier "Control"
      set modifier_indicator "Ctrl"
      set PLATFORM 2
    } elseif {[file exists /usr/bsd ]&&[file exists /usr/sbin ]} {
      # Tcl on IRIX - different grey
      set COLOR(bg) #c1c1c1
      set COLOR(abg) #bbb
      set COLOR(sbg) #aaa
      set modifier "Control"
      set modifier_indicator "Ctrl"
      set PLATFORM 3
    } else {
      # Tcl on Unix - shades of blue
      set COLOR(bg) #acf
      set COLOR(abg) #67f
      set COLOR(sbg) #33f
      set modifier "Control"
      set modifier_indicator "Ctrl"
      set PLATFORM 0
    }

    set COLOR(entry) white
    set FONT(button) -Adobe-Helvetica-Bold-R-Normal--*-120-*
    set FONT(label) -Adobe-Helvetica-Medium-R-Normal--*-120-*
    set FONT(fixed) -Adobe-Courier-Medium-R-Normal--*-120-*

    # set colors everywhere
    tk_setPalette background $COLOR(bg) \
	 activeBackground $COLOR(abg) selectBackground $COLOR(abg)

    # set directory for saving files to user's home directory
    set user_file_dir "~/"

    # close the default toplevel window
    wm withdraw .

    # set up string for title bar
    set TITLE "SaVi"

    if {$geomview_module == 1} {
	geomview_init
    }

    # Should we show the about box as a splash screen?
    if {$splash_about == 1} {
      about(build)
    }

    set realtime_flag 0
    trace variable realtime_flag w flag_change

    # default high-resolution rendering size
    set rendering_size(x) 4000
    set rendering_size(y) 4000

    set helpfile_name ""
    set helpfile_text ""
    set source_comments ""
    set details_source ""

    trace variable plane_flag w flag_change
    set plane_flag 0

    set sun_flag 0
    trace variable sun_flag w flag_change

    trace variable realtime_flag w flag_change
    set realtime_flag 0

    # load in fake sun as first satellite (zero)
    sunlight(create)

    equator(init)
    coverage(init)
    fisheye(init)
    load_url_tle(init)

    # build top window
    set last_filename ""
    main(build)
}

proc flag_change {name element op} {
    global geomview_module box_sat_flag planes_sat_flag fancy_sat_flag \
           sphere_sat_flag satellites_flag no_access_flag sun_flag \
	   simple_earth_flag fancy_earth_flag plane_flag
    upvar #0 $name flag

   if {$geomview_module == 1} {
    set not_geomview_flag 0
    if {$name == "satellites_flag"} {
	if {$flag == 1} {satellites SATELLITES_ON} else {satellites SATELLITES_OFF}
    } elseif {$name == "box_sat_flag"} {
        if {$flag == 1} {
            satellites BOX_SAT_ON
	    set fast_marker_sat_flag 0
            set planes_sat_flag 0
	    set fancy_sat_flag 0
	    set sphere_sat_flag 0
        } else {satellites BOX_SAT_OFF}
    } elseif {$name == "planes_sat_flag"} {
	if {$flag == 1} {
	    satellites PLANES_SAT_ON
	    set fast_marker_sat_flag 0
	    set box_sat_flag 0
	    set fancy_sat_flag 0
	    set sphere_sat_flag 0
	} else {satellites PLANES_SAT_OFF}
    } elseif {$name == "fancy_sat_flag"} {
        if {$flag == 1} {
	    satellites FANCY_SAT_ON
	    set fast_marker_sat_flag 0
	    set box_sat_flag 0
	    set planes_sat_flag 0
	    set sphere_sat_flag 0
	} else {satellites FANCY_SAT_OFF}
    } elseif {$name == "sphere_sat_flag"} {
        if {$flag == 1} {
	    satellites SPHERE_SAT_ON
	    set fast_marker_sat_flag 0
	    set box_sat_flag 0
	    set planes_sat_flag 0
	    set fancy_sat_flag 0
	} else {satellites SPHERE_SAT_OFF}
    } elseif {$name == "fast_marker_sat_flag"} {
        if {$flag == 1} {
	    satellites FAST_MARKER_SAT_ON
	    set sphere_sat_flag 0
	    set box_sat_flag 0
	    set planes_sat_flag 0
	    set fancy_sat_flag 0
	} else {satellites FAST_MARKER_SAT_OFF}
    } elseif {$name == "orbits_flag"} {
	if {$flag == 1} {satellites ORBITS_ON} else {satellites ORBITS_OFF}
    } elseif {$name == "footprints_flag"} {
	if {$flag == 1} {satellites FOOTPRINTS_ON} else {satellites FOOTPRINTS_OFF}
    } elseif {$name == "cones_flag"} {
	if {$flag == 1} {satellites CONES_ON} else {satellites CONES_OFF}
    } elseif {$name == "axes_flag"} {
	if {$flag == 1} {satellites AXES_ON} else {satellites AXES_OFF}
    } elseif {$name == "stars_flag"} {
	if {$flag == 1} {satellites STARS_ON} else {satellites STARS_OFF}
    } elseif {$name == "earth_flag"} {
	if {$flag == 1} {satellites EARTH_ON} else {satellites EARTH_OFF}
    } elseif {$name == "simple_earth_flag"} {
	if {$flag == 1} {
	    satellites SIMPLE_EARTH_ON
	    set fancy_earth_flag 0
	} else {satellites SIMPLE_EARTH_OFF}
    } elseif {$name == "fancy_earth_flag"} {
	if {$flag == 1} {
	    satellites FANCY_EARTH_ON
	    set simple_earth_flag 0
	} else {satellites FANCY_EARTH_OFF}
    } elseif {$name == "texture_flag"} {
	if {$flag == 1} {satellites TEXTURE_ON} else {satellites TEXTURE_OFF}
    } elseif {$name == "geomview_flag"} {
	if {$flag == 1} {satellites GEOMVIEW_ON} else {satellites GEOMVIEW_OFF}
    } elseif {$name == "geomview_dynamic_texture_flag"} {
	if {$flag == 1} {
	    satellites GEOMVIEW_DYNAMIC_TEXTURE_ON
	} else {
	    satellites GEOMVIEW_DYNAMIC_TEXTURE_OFF
	}
    } else {
	set not_geomview_flag 1
    }
    if {$not_geomview_flag == 0} return
   }

    if {$name == "realtime_flag"} {
	if {$flag == 1} {satellites REALTIME_ON} else {satellites REALTIME_OFF}
    } elseif {$name == "coverage_flag"} {
	if {$flag == 1} {satellites COVERAGE_ON} else {satellites COVERAGE_OFF}
	if {[winfo exists .coverage]} coverage(update_wells)
    } elseif {$name == "no_access_flag"} {
	if {$flag == 1} { satellites NO_ACCESS_ON
	} else {
	    if {$sun_flag == 1} {
		set sun_flag 0
	    }
	    satellites NO_ACCESS_OFF
	}
	if {[winfo exists .coverage]} coverage(update_wells)
    } elseif {$name == "print_access_flag"} {
	if {$flag == 1} {satellites PRINT_ACCESS_ON} else {satellites PRINT_ACCESS_OFF}
    } elseif {$name == "map_flag"} {
	if {$flag == 1} {
	    satellites MAP_ON
	} else {
	    satellites MAP_OFF
	}
    } elseif {$name == "plane_flag"} {
        if {$flag == 1} {
            satellites PLANE 0.0 0.0
	    equator(create)
            fisheye(update)
        } else {
            satellites PLANE_OFF
            fisheye(update)
        }

    } elseif {$name == "fisheye_reverse_view_flag"} {
	if {$flag == 1} {satellites FISHEYE_REVERSE_VIEW_ON} else {satellites FISHEYE_REVERSE_VIEW_OFF}
    } elseif {$name == "fisheye_print_inview_flag"} {
	if {$flag == 1} {satellites FISHEYE_PRINT_INVIEW_ON} else {satellites FISHEYE_PRINT_INVIEW_OFF}
    } elseif {$name == "tracks_flag"} {
	if {$flag == 1} {satellites TRACKS_ON} else {satellites TRACKS_OFF}
    } elseif {$name == "project_tracks_flag"} {
	if {$flag == 1} {satellites PROJECT_TRACKS_ON} else {satellites PROJECT_TRACKS_OFF}
    } elseif {$name == "coverage_proj_flag"} {
	if {$flag == 1} {satellites COVERAGE_PROJ 1} else {satellites COVERAGE_PROJ 0}
    } elseif {$name == "coverage_angle_flag"} {
	if {$flag == 1} {satellites COVERAGE_ANGLE_TYPE 1} else {satellites COVERAGE_ANGLE_TYPE 0}
    } elseif {$name == "fisheye_flag"} {
	if {$flag == 1} {satellites FISHEYE_ON} else {satellites FISHEYE_OFF}
    } elseif {$name == "fisheye_labels_flag"} {
        if {$flag == 1} {satellites FISHEYE_LABELS_ON} else {satellites FISHEYE_LABELS_OFF}
    } elseif {$name == "fisheye_names_flag"} {
        if {$flag == 1} {satellites FISHEYE_NAMES_ON} else {satellites FISHEYE_NAMES_OFF}
    } elseif {$name == "fisheye_viewpoint_flag"} {
        if {$flag == 1} {satellites FISHEYE_VIEWPOINT_ON} else {satellites FISHEYE_VIEWPOINT_OFF}
    } elseif {$name == "fisheye_numbers_flag"} {
        if {$flag == 1} {satellites FISHEYE_NUMBERS_ON} else {satellites FISHEYE_NUMBERS_OFF}
    } elseif {$name == "fisheye_times_flag"} {
        if {$flag == 1} {satellites FISHEYE_TIMES_ON} else {satellites FISHEYE_TIMES_OFF}
    } elseif {$name == "sun_flag"} {
	if {$flag == 1} {
	    satellites SUN_ON
	    set no_access_flag 1
	    main(show_sunlight)
	    coverage(labels_show_sunlight)
	    coverage(update)
	} else {
	    satellites SUN_OFF
	    main(hide_sunlight)
	    coverage(labels_show_decay)
	    coverage(update_decay)
	}
    } else {
	puts "Unrecognized flag: $name"
    }
}
