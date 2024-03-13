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
 * gv_init.c
 *
 * Routines for displaying satellites in geomview.
 *
 * $Id: gv_init.c,v 1.46 2017/01/02 06:11:32 lloydwood Exp $
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "Satellite.h"
#include "orbit_utils.h"
#include "gv_utils.h"
#include "savi.h"
#include "sats.h"
#include "globals.h"

#define DETAILED_TEXTUREMAP_NAME "oogl/blue_marble_land_shallow_topo_2048.jpeg"

static char tname[] = "trans_          ";

/*
 * gv_sat_create
 *
 * Creates geomview objects for the satellite
 */
void
gv_sat_create(Satellite s, const Constellation * pconstellation)
{
  unsigned int i;

  for (i = 0; i < N_VIEW_MODULES; i++)
    if (view_modules[i].display_fn)
      view_modules[i].display_fn(s, pconstellation);
}

/*
 * gv_sat_transform
 *
 * Writes a gv transform for a satellite
 */
void
gv_trans_create(Satellite s)
{
  sprintf(tname + 6, "%-10d", s->id);
  gv_transform(tname, s->transform);
}

/*
 * gv_sat_list_create
 *
 * Creates all satellites in geomview
 */
void
gv_sat_list_create(const Constellation * pconstellation)
{
  Satellite_list sl = pconstellation->satellites;

  gv_begin();
  while (sl) {
    gv_sat_create(sl->s, pconstellation);
    sl = sl->next;
  }
  gv_end();
}

/*
 * gv_trans_list_create
 *
 * Updates transformation matrices containing positions of all satellites
 */
void
gv_trans_list_create(Satellite_list sl)
{
  gv_begin();
  while (sl) {
    gv_trans_create(sl->s);
    sl = sl->next;
  }
  gv_end();
}

/*
 * provide information on missing texturemap
 */
void
gv_detailed_texturemap_info()
{
  error("can use a detailed unprojected color map, such as the maps from:");
  error("NASA's Blue Marble project  http://visibleearth.nasa.gov/");
  error("JHT's Planet Pixel Emporium http://planetpixelemporium.com/");
  fprintf(stderr, "\n      Save the jpeg colour map then copy to " DETAILED_TEXTUREMAP_NAME);
}

/*
 * load up handle file into geomview
 *
 * Returns true if successful.
 */
unsigned int
gv_init()
{
  /* call initialize proc for gv_utils */
  gv_init_proc();

  /* set gv flag on for display */
  geomview_set_flag(TRUE);

  /* abort early to avoid asking questions of non-existent Geomview */
  if (fake_geomview_module) return TRUE;

  {
    char *ver;
    static char knownlatestvers[] = "1.9.5";
    static char firstgoodX11logo[] = "1.8.2";
    static char firstgoodstreaming[] = "1.9.0";
    static char firstgoodzlibstreaming[] = "1.9.5";
    static char opengl[] = "OpenGL";

    ver = gv_version();

    /* If Geomview could be newer, tell user. */
    if (strncmp(ver, knownlatestvers, 5) < 0) {
      error_format("Geomview %s is available from "
	           "http://www.geomview.org/", knownlatestvers);
      error_format("but you are still using Geomview %s. Please upgrade!\n", ver);
    }

    if (!strstr(ver, opengl)) {
      /*
       * Figure out if we have a buggy version of Geomview which draws
       * the logo upside down!
       */
      if (strncmp(firstgoodX11logo, ver, 5) > 0) {
        geomview_logo = FALSE;
        error("not drawing logo under Geomview's buggy X11 rendering. Upgrade!");
      }
      error("can use OpenGL for drawing coverage maps in Geomview.");
      error("configure Geomview with OpenGL (--with-opengl).\n");
    }

    if (strncmp(firstgoodstreaming, ver, 5) > 0) {
      geomview_stream_textures = FALSE;
      error("can't stream dynamic coverage maps to this old Geomview. Upgrade!");
    } else {
      if ((debug && strncmp(firstgoodzlibstreaming, ver, 5) > 0)) {
	geomview_compress2_textures = FALSE;
	error("can't talk zlib direct down a pipe to this version of Geomview.");
	error("falling back on older Geomview-spawns-gzip method used by 1.9.4 and older.");
      }
      {
	FILE *f;
        if (NULL == (f = fopen(DETAILED_TEXTUREMAP_NAME, "r"))) {
          gv_detailed_texturemap_info();
        } else {
          fclose(f);
	  geomview_detailed_texturemap = TRUE;
        }
      }
    }
  }

  /* load up savi base oogl file */
  if (!gv_sendfile(HANDLE_FILE_SAVI)) {
    error_format("gv_init: Can't load file %s", HANDLE_FILE_SAVI);
    error("file telling Geomview about Earth and satellites is not there!");
    return FALSE;
  }

  if (!geomview_logo) {
    return TRUE;
  }

  /* load up savi logo file */
  if (!gv_sendfile(HANDLE_FILE_LOGO)) {
    error_format("gv_init: Can't load file %s", HANDLE_FILE_LOGO);
    error("not drawing SaVi logo in Geomview camera window.");
    geomview_logo = FALSE;
    return TRUE;
  }

  /* turn on logo */
  logo_on_cmd(0, NULL);

  return TRUE;
}
