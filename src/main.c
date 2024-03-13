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
 * main.c
 *
 * $Id: main.c,v 1.94 2017/06/26 04:46:33 lloydwood Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "utils.h"
#include "constants.h"
#include "coverage_vis.h"
#include "globals.h"
#include "sats.h"
#include "savi.h"
#include "tcl_utils.h"
#include "gv_utils.h"

static unsigned int check_env_vars(void);
static void decode_cmd_line(int argc, char *argv[]);

int
main(int argc, char *argv[])
{
  char buf[] = "main(update)";

  /* decode command line */
  decode_cmd_line(argc, argv);

  /* initialize geomview if started as a module */
  if (geomview_module) {
    if (!gv_stream_init_proc()) {
      error("could not open pipe to Geomview; savi is now running standalone.");
      geomview_module = FALSE;
    } else if (!gv_init()) {
      error("could not initialise Geomview (main: error in gv_init).");
      geomview_module = FALSE;
    } else {
      GV_BINARY_FORMAT_AVAILABLE = gv_init_proc();
    }
  }

  if (!check_env_vars()) {
    error("edit and run the savi wrapper script in savi's directory to set locations.");
  }

  Tcl_FindExecutable(argv[0]);

  interp = Tcl_CreateInterp();

  /* initialize Tcl interface */
  if (!tcl_init(interp)) {
    error_and_exit("could not initialise Tcl (main: error in tcl_init).");
  }

  /* initialize Tk interface */
  if (!tk_init(interp)) {
    error_and_exit("could not initialise Tk (main: error in tk_init).");
  }

  /* load up a single satellite */
  if (!first_filename) {
      satellite_init();
  }
  /* initialise satellites */
  sats_init();

  /* and update interface */
  if (!tcl_script(buf)) {
    error_and_exit("is SaVi compiled correctly? "
		   "Edit savi startup scripts to set locations.");
  }

  /* loop until exit */
  while (sats_update() && tk_update());

  Tcl_DeleteInterp(interp);

  exit(0);
}

static unsigned int
check_env_vars(void)
{
  unsigned int all_env_found = TRUE;

  if (!getenv("SAVI")) {
    error("$SAVI was not set to location of savi directory containing launch script.");
    all_env_found = FALSE;
  }

  if (!getenv("TCL_LIBRARY")) {
    error("$TCL_LIBRARY was not set to location of directory containing Tcl.");
    all_env_found = FALSE;
  }

  if (!getenv("TK_LIBRARY")) {
    error("$TK_LIBRARY was not set to location of directory containing Tk.");
    all_env_found = FALSE;
  }

  return all_env_found;
}

static void
decode_cmd_line(int argc, char *argv[])
{
  unsigned int unknown_parameters = 0;
  unsigned int skipped_filenames = 0;
  unsigned int more_filenames = 0;
  unsigned int special_flags_done = 0;
  unsigned int length_filename = 0;

  char temp_buffer[LENGTH_STRING_BUFFER];
  char temp_switches[LENGTH_STRING_BUFFER];
  char *switches = temp_switches;
  char *command_line = command_switches;
  char *script_filename = NULL;
  FILE *script_file = NULL;

  int give_version = FALSE;
  int give_help = FALSE;
  int coverage_large_map = FALSE;
  int not_fake_geomview = FALSE;
  int levels_error = FALSE;
  int levels_format_error = FALSE;
  int orbit_model_unknown = FALSE;
  int map_longitude_error = FALSE;
  int map_format_error = FALSE;
  int map_view_error = FALSE;
  int map_view_height = 0;
  int min_transmit_altitude_error = FALSE;
  int max_transmit_altitude_error = FALSE;
  int length_savi_name, length_switches;
  unsigned int i;

  int can_use_zlib = capable_of_compression();

  for (i = 1; i < argc; i++) {
    /* argv[0] gives name of binary, not wrapper script */
    if (strcmp(argv[i], "-geomview") == 0) {
      not_fake_geomview = TRUE;
      geomview_module = TRUE;
    } else if (strcmp(argv[i], "-fake-geomview") == 0) {
      geomview_module = TRUE;
      fake_geomview_module = TRUE;
    } else if (strcmp(argv[i], "-debug") == 0) {
      debug = TRUE;
    } else if (strcmp(argv[i], "-help") == 0) {
      special_flags_done++;
      give_help = TRUE;
    } else if (strcmp(argv[i], "-large-map") == 0) {
      coverage_large_map = TRUE;
    } else if (strcmp(argv[i], "-dynamic-texture-with-map") == 0) {
      geomview_texture_with_map = TRUE;
    } else if (strcmp(argv[i], "-gzip-compressed-textures") == 0) {
      geomview_compress2_textures = FALSE;
    } else if (strcmp(argv[i], "-uncompressed") == 0) {
      geomview_compressed_images = FALSE;
    } else if (strcmp(argv[i], "-no-logo") == 0) {
      geomview_logo = FALSE;
    } else if (strcmp(argv[i], "-redrawn-menus") == 0) {
      buttons_menu = TRUE;
    } else if (strcmp(argv[i], "-version") == 0) {
      special_flags_done++;
      give_version = TRUE;
    } else if (strcmp(argv[i], "-splash") == 0) {
      splash_about = TRUE;
    } else if (strcmp(argv[i], "-sun-lighting") == 0) {
      geomview_sun_lighting = TRUE;
    } else if (strcmp(argv[i],"-levels") == 0) {
      if (++i < argc) {
	if (sscanf((char *) argv[i], "%i", &NUM_COLORS) == 1) {
	  if ((NUM_COLORS < 1) ||
	      (NUM_COLORS > 19)) {
	    NUM_COLORS = 0;
	    levels_error = TRUE;
          }
	} else {
	  levels_format_error = TRUE;
	  i--;
	}
      }
    } else if (strcmp(argv[i],"-orbit-model") == 0) {
      if (++i < argc) {
        if (strcmp(argv[i],"J0") == 0) {
          orbit_model = J0;
        } else if (strcmp(argv[i],"J2") == 0) {
          orbit_model = J2;
        } else {
          orbit_model_unknown = TRUE;
	  i--;
        }
      } else {
        orbit_model_unknown = TRUE;
      }
    } else if (strcmp(argv[i],"-map-view-middle") == 0) {
      if (++i < argc) {
	if (sscanf((char *) argv[i], "%i", &coverage_display_center_longitude) == 1) {
	  if ((coverage_display_center_longitude < -180) ||
	      (coverage_display_center_longitude > 180)) {
	    coverage_display_center_longitude = 0;
	    map_longitude_error = TRUE;
          }
	} else {
	  map_format_error = TRUE;
	  i--;
	}
      }
    } else if (strcmp(argv[i],"-map-view-height") == 0) {
      if (++i < argc) {
	if (sscanf((char *) argv[i], "%i", &map_view_height) != 1) {
	  map_view_error = TRUE;
	  i--;
	}
      }
    } else if (strcmp(argv[i],"-min-transmit-altitude") == 0) {
      if (++i < argc) {
        if (sscanf((char *) argv[i], "%i", &min_transmit_altitude) != 1) {
          min_transmit_altitude_error = TRUE;
          i--;
        }
      }
    } else if (strcmp(argv[i],"-max-transmit-altitude") == 0) {
      if (++i < argc) {
        if (sscanf((char *) argv[i], "%i", &max_transmit_altitude) != 1) {
          max_transmit_altitude_error = TRUE;
          i--;
        }
      }
    } else if (strncmp(argv[i],"-",1) == 0) {
      unknown_parameters++;
    } else if (!script_filename) {
      if (NULL == (script_file = fopen(argv[i], "r"))) {
        skipped_filenames++;
      } else {
        script_filename = argv[i];
        fclose(script_file);
	length_filename = strlen(script_filename);
      }
    } else if (!more_filenames) {
      error("already found specified file. Ignoring any additional filenames.");
      more_filenames++;
    }
  }

  if (!can_use_zlib) {
    /* turn off all compression flags */
    geomview_compress2_textures = FALSE;
    geomview_compressed_images = FALSE;
  }

  if (give_version) {
    fprintf(stderr,"\n%s\n",Version);
    sprintf(temp_buffer," -version");
    sprintf(switches, temp_buffer);
    switches += strlen(temp_buffer);
  }

  if (unknown_parameters) {
    if (unknown_parameters == 1) {
      error("Unknown flag passed to SaVi.");
    } else {
      error("Multiple unknown flags passed to SaVi.");
    }
    give_help = TRUE;
  }

  if (give_help) {
    error("supported command-line options when SaVi is run standalone include:\n"
	  "savi [-large-map] [-levels <n>] [-debug] [-fake-geomview] [-redrawn-menus]\n"
	  "     [-map-view-height <no. of pixels>] [-map-view-middle <longitude>] \n"
          "     [-max-transmit-altitude <kilometers>] [-min-transmit-altitude <kilometers>]\n"
	  "     [-orbit-model <J0|J2>]  [filename]");
    error("further flags become available when SaVi is run from Geomview:\n"
	  "savi [-geomview] [-dynamic-texture-with-map] [-gzip-compressed-textures]\n"
	  "     [-uncompressed] [-no-logo] [-sun-lighting]");
    error("other useful flags:\n"
	  "savi -help repeats this help information at the command line.\n"
          "savi -version provides version information at the command line.\n"
	  "savi -splash shows copyright and version information in a window.\n");
    sprintf(temp_buffer," -help");
    sprintf(switches, temp_buffer);
    switches += strlen(temp_buffer);
  }

  if (special_flags_done &&
      (special_flags_done == argc - unknown_parameters - 1)) {
    /*
     * exit only if only help or version information was requested,
     * but continue if they were requested along with any other valid flag.
     */
    exit(0);
  }

  if (skipped_filenames) {
    if (skipped_filenames > 1) {
      error("couldn't find multiple files passed as parameters.");
    } else {
      error("couldn't find file passed as parameter.");
    }
  }

  if (script_filename) {
    first_filename = script_filename;
    /*
     * Only display filename if multiple filenames given means ambiguity.
     */
    if (skipped_filenames || more_filenames) {
      error("loading specified file:");
      error(script_filename);
    }
  }

  if (!geomview_module) {
    error("For 3D visualization, run from within Geomview.");
    if (!geomview_logo) {
      error("logo control is for Geomview.");
      geomview_logo = TRUE;
    }
    if (geomview_texture_with_map) {
      error("dynamic texture mapping only under Geomview.");
      geomview_dynamic_texture_flag = FALSE;
      geomview_texture_with_map = FALSE;
    }
    if (geomview_sun_lighting) {
      error("sun lighting control is only in Geomview.");
      geomview_sun_lighting = FALSE;
    }
  }

  if (fake_geomview_module) {
    if (!not_fake_geomview) {
      error("Enabling all features that normally require Geomview.");
      sprintf(temp_buffer," -fake-geomview");
      sprintf(switches, temp_buffer);
      switches += strlen(temp_buffer);
    } else {
      error("-geomview overrides -fake-geomview. Waiting for Geomview.");
      fake_geomview_module = FALSE;
    }
  }

  if (!can_use_zlib) {
    error("compiled without zlib compression.");
  }

  if (levels_error) {
    error("Only 1 to 19 levels of color shading are supported.");
  }

  if (levels_format_error) {
    error("format is -levels <levels of color shading>. Default is 4.");
  }

  if (NUM_COLORS) {
    if (NUM_COLORS == 1) {
      error("one level of color shading selected for all coverage.");
    } else {
      fprintf(stderr, "\nSaVi: %i levels of color shading selected.",
	      NUM_COLORS);
    }
    sprintf(temp_buffer," -levels %i", NUM_COLORS);
    sprintf(switches, temp_buffer);
    switches += strlen(temp_buffer);
  }

  if (orbit_model_unknown) {
    error("unfamiliar orbit model. format is -orbit-model <J0|J2>. Selecting J2.");
  } else if (orbit_model == J0) {
    error("selected J0 orbital model for classical mechanics.");
    sprintf(temp_buffer, " -orbit-model J0");
    sprintf(switches, temp_buffer);
    switches += strlen(temp_buffer);
  } else if (orbit_model == J2) {
    error("defaulting to J2 orbital model.");
    sprintf(temp_buffer, " -orbit-model J2");
    sprintf(switches, temp_buffer);
    switches += strlen(temp_buffer);
  }

  if (map_format_error) {
    error("format is -map-view-middle <longitude>");
  }

  if (map_longitude_error) {
    error("longitude given for middle of map view is invalid.");
  }

  if (map_view_error) {
    error("format is -map-view-height <number of pixels>");
  }
  if (min_transmit_altitude_error) {
    error("format is -min-transmit-altitude <kilometers>");
  }
  if (max_transmit_altitude_error) {
    error("format is -max-transmit-altitude <kilometers>");
  }

  if (min_transmit_altitude > 0) {
    sprintf(temp_buffer, " -min-transmit-altitude %i", min_transmit_altitude);
    sprintf(switches, temp_buffer);
    switches += strlen(temp_buffer);
  } else {
    min_transmit_altitude = -1;
  }

  if (max_transmit_altitude > 0) {
    sprintf(temp_buffer, " -max-transmit-altitude %i", max_transmit_altitude);
    sprintf(switches, temp_buffer);
    switches += strlen(temp_buffer);
  } else {
    max_transmit_altitude = -1;
  }

  if (buttons_menu) {
    sprintf(temp_buffer, " -redrawn-menus");
    sprintf(switches, temp_buffer);
    switches += strlen(temp_buffer);
  }

  if (debug) {
    sprintf(temp_buffer, " -debug");
    sprintf(switches, temp_buffer);
    switches += strlen(temp_buffer);
  }

  if (coverage_custom_height_valid(map_view_height)) {
    Image_Height = map_view_height;
    Image_Width = 2 * Image_Height;
    fprintf(stderr, "\nSaVi: coverage map sized to %i by %i pixels.",
	    Image_Width, Image_Height);
    sprintf(temp_buffer," -map-view-height %i", map_view_height);
    sprintf(switches, temp_buffer);
    switches += strlen(temp_buffer);
  } else if (coverage_large_map) {
      Image_Width = IMAGE_LARGE_WIDTH;
      Image_Height = IMAGE_LARGE_HEIGHT;
      error("large default map size selected.");
      sprintf(temp_buffer," -large-map");
      sprintf(switches, temp_buffer);
      switches += strlen(temp_buffer);
  }

  if (coverage_display_center_longitude) {
    sprintf(temp_buffer, " -map-view-middle %i", coverage_display_center_longitude);
    sprintf(switches, temp_buffer);
    switches += strlen(temp_buffer);
  }

  if (geomview_module) {
    sprintf(temp_buffer, " -geomview");
    sprintf(switches, temp_buffer);
    switches += strlen(temp_buffer);

    if (geomview_texture_with_map) {
      sprintf(temp_buffer, " -dynamic-texture-with-map");
      sprintf(switches, temp_buffer);
      switches += strlen(temp_buffer);
    }

    if (!geomview_compress2_textures && geomview_compressed_images) {
      sprintf(temp_buffer, " -gzip-compressed-textures");
      sprintf(switches, temp_buffer);
      switches += strlen(temp_buffer);
    }

    if (!geomview_compressed_images && can_use_zlib) {
      sprintf(temp_buffer, " -uncompressed");
      sprintf(switches, temp_buffer);
      switches += strlen(temp_buffer);
    }

    if (!geomview_logo) {
      sprintf(temp_buffer, " -no-logo");
      sprintf(switches, temp_buffer);
      switches += strlen(temp_buffer);
    }

    if (geomview_sun_lighting) {
      error("buggy sun lighting positioning enabled under Geomview.");
      sprintf(temp_buffer, " -sun-lighting");
      sprintf(switches, temp_buffer);
      switches += strlen(temp_buffer);
    }
  }
  
  length_savi_name = strlen(argv[0]);
  length_switches = switches - temp_switches;

  /* leave room for terminator */
  if (length_savi_name + length_switches <
      LENGTH_STRING_BUFFER - 1) {
    sprintf(command_line, argv[0]);
    command_line += length_savi_name;

    if (length_switches) {
      /* presumes selected switches are of controlled length less than buffer */
      sprintf(command_line, temp_switches);
      command_line += length_switches;
    }
  }

  /* leave room for terminator */
  i = length_savi_name + length_switches + length_filename;
  if (script_filename &&
      (i < LENGTH_STRING_BUFFER - 1)) {
    sprintf(command_line, " %s", script_filename);
    command_line += length_filename + 1;
  }

  /* terminate the string with null char, however long it is. */
  *command_line = 0;
}
