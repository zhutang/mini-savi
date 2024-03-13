/*
 *****************************************************
 *
 *  SaVi by Lloyd Wood (lloydwood@users.sourceforge.net),
 *          Patrick Worfolk (worfolk@alum.mit.edu) and
 *          Robert Thurman.
 *
 *  Copyright (c) 1997 by The Geometry Center.
 *  Also Copyright (c) 2017 by Lloyd Wood.
 *
 *  This file is part of SaVi.  SaVi is free software;
 *  you can redistribute it and/or modify it only under
 *  the terms given in the file COPYRIGHT which you should
 *  have received along with this file.  SaVi may be
 *  obtained from:
 *  http://savi.sourceforge.net/
 *  http://www.geom.uiuc.edu/locate/SaVi
 *
 *****************************************************
 *
 * satCmd.c
 *
 * procedure to implement tcl command "satellites"
 *
 * $Id: satCmd.c,v 1.38 2017/01/02 06:11:32 lloydwood Exp $
 */

#include <string.h>
#include <stdio.h>
#include <tcl.h>

#include "fisheye.h"
#include "globals.h"
#include "constants.h"
#include "utils.h"
#include "gv_utils.h"
#include "sats.h"
#include "savi.h"
#include "tcl_utils.h"

extern Tcl_Interp *interp;	/* Interpreter for this application. */

/*
 * define structure for the string to keyword translation
 */
typedef char *(*PCmdF) (int argc, char *argv[]);
struct Key
{
  const char *word;
  PCmdF cmd_fn;
};

static PCmdF keyword_to_fn(const char *word);

/*
 * define the set of keywords and command functions
 *  - these correspond to the allowable commands "satellite COMMAND ..."
 */
static struct Key key[] = {
  /* tape recorder commands */
  {"FORWARDS", forwards_cmd},
  {"BACKWARDS", backwards_cmd},
  {"FORW_STEP", forwards_step_cmd},
  {"BACK_STEP", backwards_step_cmd},
  {"RESET", reset_cmd},
  {"STOP", stop_cmd},

  /* option commands */
  {"SATELLITES_ON", satellites_on_cmd},
  {"SATELLITES_OFF", satellites_off_cmd},
  {"BOX_SAT_ON", box_sat_on_cmd},
  {"BOX_SAT_OFF", box_sat_off_cmd},
  {"PLANES_SAT_ON", planes_sat_on_cmd},
  {"PLANES_SAT_OFF", planes_sat_off_cmd},
  {"SPHERE_SAT_ON", sphere_sat_on_cmd},
  {"SPHERE_SAT_OFF", sphere_sat_off_cmd},
  {"FAST_MARKER_SAT_ON", fast_marker_sat_on_cmd},
  {"FAST_MARKER_SAT_OFF", fast_marker_sat_off_cmd},
  {"FANCY_SAT_ON", fancy_sat_on_cmd},
  {"FANCY_SAT_OFF", fancy_sat_off_cmd},
  {"AXES_ON", axes_on_cmd},
  {"AXES_OFF", axes_off_cmd},
  {"LOGO_ON", logo_on_cmd},
  {"LOGO_OFF", logo_off_cmd},
  {"ORBITS_ON", orbits_on_cmd},
  {"ORBITS_OFF", orbits_off_cmd},
  {"ORBIT_SET", orbits_set_cmd},
  {"CONES_ON", cones_on_cmd},
  {"CONES_OFF", cones_off_cmd},
  {"COVERAGE_SET", coverage_set_cmd},
  {"FOOTPRINTS_ON", footprints_on_cmd},
  {"FOOTPRINTS_OFF", footprints_off_cmd},
  {"EARTH_ON", earth_on_cmd},
  {"EARTH_OFF", earth_off_cmd},
  {"SIMPLE_EARTH_ON", simple_on_cmd},
  {"SIMPLE_EARTH_OFF", simple_off_cmd},
  {"FANCY_EARTH_ON", fancy_on_cmd},
  {"FANCY_EARTH_OFF", fancy_off_cmd},
  {"TEXTURE_ON", texture_on_cmd},
  {"TEXTURE_OFF", texture_off_cmd},
  {"SUN_ON", sun_on_cmd},
  {"SUN_OFF", sun_off_cmd},
  {"STARS_ON", stars_on_cmd},
  {"STARS_OFF", stars_off_cmd},
  {"PLANE", plane_cmd},
  {"PLANE_OFF", plane_off_cmd},
  {"GEOMVIEW_ON", geomview_on_cmd},
  {"GEOMVIEW_OFF", geomview_off_cmd},
  {"GEOMVIEW_DYNAMIC_TEXTURE_ON", geomview_dynamic_texture_on_cmd},
  {"GEOMVIEW_DYNAMIC_TEXTURE_OFF", geomview_dynamic_texture_off_cmd},
  {"REALTIME_ON", realtime_on_cmd},
  {"REALTIME_OFF", realtime_off_cmd},
  {"COVERAGE_ON", coverage_on_cmd},
  {"COVERAGE_OFF", coverage_off_cmd},
  {"NO_ACCESS_ON", no_access_on_cmd},
  {"NO_ACCESS_OFF", no_access_off_cmd},
  {"MAP_ON", map_on_cmd},
  {"MAP_OFF", map_off_cmd},
  {"PRINT_ACCESS_ON", print_access_on_cmd},
  {"PRINT_ACCESS_OFF", print_access_off_cmd},
  {"FISHEYE_REVERSE_VIEW_ON", fisheye_reverse_view_on_cmd},
  {"FISHEYE_REVERSE_VIEW_OFF", fisheye_reverse_view_off_cmd},
  {"FISHEYE_PRINT_INVIEW_ON", fisheye_print_inview_on_cmd},
  {"FISHEYE_PRINT_INVIEW_OFF", fisheye_print_inview_off_cmd},
  {"FISHEYE_NAMES_ON", fisheye_names_on_cmd},
  {"FISHEYE_NAMES_OFF", fisheye_names_off_cmd},
  {"FISHEYE_VIEWPOINT_ON", fisheye_viewpoint_on_cmd},
  {"FISHEYE_VIEWPOINT_OFF", fisheye_viewpoint_off_cmd},
  {"FISHEYE_NUMBERS_ON", fisheye_numbers_on_cmd},
  {"FISHEYE_NUMBERS_OFF", fisheye_numbers_off_cmd},
  {"FISHEYE_TIMES_ON", fisheye_times_on_cmd},
  {"FISHEYE_TIMES_OFF", fisheye_times_off_cmd},
  {"TRACKS_ON", tracks_on_cmd},
  {"TRACKS_OFF", tracks_off_cmd},
  {"PROJECT_TRACKS_ON", project_tracks_on_cmd},
  {"PROJECT_TRACKS_OFF", project_tracks_off_cmd},
  {"COVERAGE_PROJ", coverage_proj_cmd},
  {"COVERAGE_ANGLE_TYPE", coverage_angle_type_cmd},
  {"COVERAGE_SAVE_MAP", coverage_save_map_cmd},
  {"FISHEYE_ON", fisheye_on_cmd},
  {"FISHEYE_OFF", fisheye_off_cmd},
  {"FISHEYE_LABELS_ON", fisheye_labels_on_cmd},
  {"FISHEYE_LABELS_OFF", fisheye_labels_off_cmd},
  {"FISHEYE_SET", fisheye_set_cmd},

  /* editing commands */
  {"NEW", sats_new_cmd},
  {"COPY", sats_copy_cmd},
  {"DELETE", sats_delete_cmd},
  {"DELETE_ALL", sats_delete_all_cmd},
  {"GET", sats_get_cmd},
  {"SET", sats_set_cmd},
  {"TAG", sats_tag_cmd},
  {"UNTAG", sats_untag_cmd},
  {"LOAD_MARKER", sats_load_marker_cmd},
  {"LOAD", sats_load_cmd},
  {"NAME", sats_name_cmd},
  {"GET_NAME", sats_get_name_cmd},
  {"GET_ID", sats_get_id_cmd},

  /* geomview commands */
  {"GV_BEGIN", gv_begin_cmd},
  {"GV_END", gv_end_cmd},
  {"GV_WAIT", gv_wait_cmd},
  {"GV_SEND", gv_send_cmd},

  /* other */
  {"COLOR_UPDATE", coverage_color_update_cmd},
  {"EXIT", exit_cmd},
  {"RESET_PARAMS", reset_params_cmd},
  {"VERSION", version_cmd},
  {"COMMAND_LINE", command_line_cmd}
};

unsigned int N_KEYS = sizeof(key) / sizeof(struct Key);

/*
 * keyword_to_fn
 *   translates from key string to key command function
 */
static PCmdF
keyword_to_fn(const char *word)
{
  unsigned int i;

  for (i = 0; i < N_KEYS; i++) {
    if (!strcmp(word, key[i].word)) {
      return (key[i].cmd_fn);
    }
  }
  return (NULL);
}

/*
 * satCmd
 *
 * procedure that tcl calls to execute the "satellites" or "savi" command
 *
 */
int
satCmd(ClientData clientData, Tcl_Interp * interpreter, int argc, char *argv[])
{
  PCmdF cmd_fn = NULL;

  /* for debugging purposes, display commands */
  if (debug) {
    unsigned int i;
    for (i = 0; i < argc; i++) {
      fprintf(stderr, " %s", argv[i]);
    }
    fprintf(stderr, "\n");
  }

  if (argc < 2) {
    error("satCmd: satellite command needs an argument!");
  } else {
    cmd_fn = keyword_to_fn(argv[1]);
    if (cmd_fn) {
      Tcl_SetResult(interpreter, cmd_fn(argc, argv), TCL_STATIC);
    } else {
      error_format("satCmd %s unrecognized", argv[1]);
    }
  }

  return TCL_OK;
}
