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
 * globals.c
 *
 * declaration and initialization of global variables.
 *
 * $Id: globals.c,v 1.51 2017/06/07 21:34:22 lloydwood Exp $
 */

#include <limits.h>

#include "constants.h"
#include "globals.h"

/* Is program running as a Geomview module? */
int geomview_module = FALSE;
int fake_geomview_module = FALSE;
int geomview_logo = TRUE;

/* has texturemapping been turned on? */
int texture_flag = TRUE;

/* has dynamic texturemapping been selected? */
int geomview_dynamic_texture_flag = FALSE;
int geomview_texture_with_map = FALSE;
int geomview_detailed_texturemap = FALSE;
int geomview_stream_textures = TRUE; /* works on Geomview >= 1.9.0 */
int geomview_compress2_textures = TRUE; /* works on Geomview >= 1.9.5 */

/* does the user want uncompressed files? Assume we're capable until
   we find otherwise. */
int geomview_compressed_images = TRUE;

/* turn off geomview's buggy light source positioning */
int geomview_sun_lighting = FALSE;

/* footprints are turned off by default */
int footprints_flag = FALSE;

/* sunlight is turned off by default */
int sun_flag = FALSE;

/* equatorial plane is turned off by default */
int plane_flag = FALSE;

/* fisheye default is not open, and location is not plotted in coverage */
int fisheye_viewpoint_flag = FALSE;

/* debug flag to avoid recompiling. Off by default. */
int debug = FALSE;

/* Don't show about box as splash screen by default */
int splash_about = FALSE;

/* Default to new menu-drawing method, localised for Cygwin and Mac OS X */
/* Known to fail on Mac OS X 10.5 Leopard, hence this fallback method */
int buttons_menu = FALSE;

unsigned char * cyl_foreground = NULL;
unsigned char * sin_foreground = NULL;
unsigned char * unp_foreground = NULL;
unsigned char * unp_mask_foreground = NULL;

/* presumes default cylindrical projection */
int Longitude_Center_Line = 0;
int coverage_display_center_longitude = 0;

/* so we know if it has been set at command line. */
int NUM_COLORS = 0;

/* useful for elliptical orbits. */
int min_transmit_altitude = -1;
int max_transmit_altitude = -1;

char * first_filename = NULL;

/* empty string */
char EMPTY_str[] = "";

/* default values for panels and initial motion */
unsigned int motion = FALSE;
unsigned int reset = FALSE;
unsigned int single_step = FALSE;

double direction = 1.0;
double coverage_angle = COVERAGE_ANGLE;
int coverage_type = MASK_ELEVATION;
int orbit_model = J2;
double tracks_interval = 0;	/* in hours */
unsigned int transforms_needed = 0;

/* global consts - to link to tcl vars */
int Image_Width = 0;
int Image_Height = 0;
int Fisheye_Diameter = FISHEYE_DIAMETER;
