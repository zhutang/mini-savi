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
 * globals.h
 *
 * Global variable declarations
 *
 * $Id: globals.h,v 1.58 2017/06/07 21:34:22 lloydwood Exp $
 */

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#define LENGTH_STRING_BUFFER 256

char cmd[LENGTH_STRING_BUFFER];

char command_switches[LENGTH_STRING_BUFFER];

int geomview_module; /* TRUE if program is a geomview module */
int fake_geomview_module; /* TRUE if pretending to run under geomview */
int geomview_logo; /* TRUE if SaVi logo is shown on camera */

int texture_flag; /* TRUE if we're texturemapping */
int geomview_detailed_texturemap; /* TRUE if file available */
int geomview_dynamic_texture_flag; /* TRUE if sending coverage to Geomview. */
int geomview_texture_with_map; /* TRUE if bitmap earthmap included */
int geomview_stream_textures; /* TRUE if we don't need a scratchfile */
int geomview_compress2_textures; /* TRUE if Geomview doesn't have to spawn gzip */

int geomview_compressed_images; /* FALSE if no zlib or -uncompressed. */

int geomview_sun_lighting; /* TRUE if we trust geomview's light sources */

int footprints_flag; /* TRUE if we want to see edges, inc sun terminator */
int plane_flag; /* TRUE if we want to see equatorial plane */
int sun_flag; /* TRUE if we want to see sun lighting */
int fisheye_viewpoint_flag; /* TRUE if we want to plot location on coverage */

int debug;
int splash_about;
int buttons_menu;

typedef enum {
  MASK_ELEVATION=0, SATELLITE_CONE
} coverage_types;

typedef enum {
  J0=0, J2
} orbit_models;

int orbit_model;

/* We don't yet have a proper map for SINUSOIDAL centred on 0 deg lat. */
typedef enum {
  UNPROJECTED_MASK=0,
  UNPROJECTED,
  CYLINDRICAL,
  SINUSOIDAL,
  SINUSOIDAL_90,
  SPHERICAL,
  SPHERICAL_90,
  NUM_PROJECTIONS
} coverage_projections;

int coverage_projection;

/*
 * sinusoidal and cylindrical projections can be centered on America
 * and rotated -90 degrees
 * cylindrical and unprojected projections are normal - 0 degrees rotation.
 * Just stretch the coverage window sideways to see tiling.
 */
int Longitude_Center_Line;
int coverage_display_center_longitude;

int min_transmit_altitude;
int max_transmit_altitude;

/* number of colors in use by coverage panel map display */
int NUM_COLORS;

char * first_filename;

extern char EMPTY_str[];  /* empty string */
char *Version; /* string containing who and when compiled */

unsigned int motion; /* TRUE to move satellites, FALSE to stop them */
unsigned int reset; /* TRUE to reset satellites to original positions */
unsigned int single_step; /* TRUE if take only one step */

double equatorial_exclusion_angle; /* half-width/highest latitude of geo frequency exclusion belt */
double parallels_angle; /* angle marked out in fisheye for parallel lines of latitude */

double delta_t; /* time increment */
double coverage_angle; /* angle for footprints */
int coverage_type; /* mask elevation or the less-used cone angle */
double tracks_interval; /* time interval for computing ground tracks */
unsigned int transforms_needed; /* flag for sending transforms to gv */

/* size of image for coverage display */
int Image_Width;
int Image_Height;

/* size of fisheye */
int Fisheye_Diameter;

/* coverage colors */
int DIV0, DIV1, DIV2, DIV3, DIV4, DIV5, DIV6, DIV7, DIV8, DIV9;
int DIV10, DIV11, DIV12, DIV13, DIV14, DIV15, DIV16, DIV17, DIV18, DIV19;

int DEC0, DEC1, DEC2, DEC3, DEC4, DEC5, DEC6, DEC7, DEC8, DEC9;
int DEC10, DEC11, DEC12, DEC13, DEC14, DEC15, DEC16, DEC17, DEC18, DEC19;

#endif
/* !_GLOBALS_H_ */
