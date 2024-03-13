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
 * earth.c
 *
 * Routines for creating fancy earth model
 *
 * $Id: earth.c,v 1.45 2017/01/02 06:11:32 lloydwood Exp $
 */
#include <stdio.h>
#include <math.h>

#include "gv_utils.h"
#include "utils.h"
#include "constants.h"
#include "globals.h"
#include "coverage_vis.h"
#include "savi.h"

unsigned int earth_on_flag = FALSE;
unsigned int earth_geom_exists = FALSE;
unsigned int use_simple_earth = FALSE;
unsigned int use_fancy_earth = FALSE;


unsigned int geomview_dynamic_coverage = FALSE;

static unsigned int earth_vectors_loaded = FALSE;
static unsigned int earth_texture_loaded = FALSE;
static unsigned int earth_created = FALSE;

static double m[4][4]; /* identity matrix used for rotating Earth */

static void earth_create_geom(void);
static void earth_delete_geom(void);

/*
 * earth_create_geom
 */
static void
earth_create_geom(void)
{
  if (earth_geom_exists)
    return;

  /*
   * initialise the identity matrix
   */
  identity(m);

  if (geomview_module) {
    /* create a simple blue sphere for the earth */
    gv_start();
    gv_create_geom("Central_Body", "central_t", "unit_sphere_h");
    gv_stop();
  }
  earth_geom_exists = TRUE;
}

/*
 * earth_delete_geom
 */
static void
earth_delete_geom(void)
{
  if (!earth_geom_exists)
    return;

  if (geomview_module) {
    gv_start();
    gv_delete_geom("Central_Body");
    gv_stop();
  }
  earth_geom_exists = FALSE;
}

/*
 * create_earth
 */
void
create_earth(void)
{
  /* coverage map checkbox determines if we draw Earth with vector map or not. */
  if (get_map_flag()) {
     gv_create_geom("Central_Body", "central_t", "earth_vect_texture_sphere_h");
  } else {
     gv_create_geom("Central_Body", "central_t", "earth_h");
  }
}

/*
 * redraw_earth
 */
void
redraw_earth(int argc, char *argv[])
{
  if (earth_on_flag) {
    char *status = EMPTY_str;
    if (geomview_module) gv_start();
    status = earth_off_cmd(argc, argv);
    status = earth_on_cmd(argc, argv);
    if (geomview_module) gv_stop();
  }
}

/*
 * earth_on_cmd
 *
 * Displays the earth sphere, dull or detailed.
 */
char *
earth_on_cmd(int argc, char *argv[])
{
  char *status = EMPTY_str;

  if (earth_on_flag) return status;

  if (!earth_created) {
    /* just draw sphere */
    earth_create_geom();
  }

  earth_on_flag = TRUE;

  if (geomview_module) {
    gv_start();

    if (texture_flag) {
      if (!earth_texture_loaded) {
	earth_texture_loaded = gv_sendfile("oogl/earth_texture_h.oogl");
      }
      if (geomview_stream_textures && geomview_dynamic_texture_flag) {
	if ((coverage_projection == UNPROJECTED)||(coverage_projection == UNPROJECTED_MASK)) {
	  if (debug) error("texturemap sphere, dynamic unprojected map.");
	  gv_sendfile("oogl/earth_coverage_unp_h.oogl");
	  redraw_coverage_display();
	} else if (coverage_projection == CYLINDRICAL) {
	  if (debug) error("texturemap sphere, dynamic cylindrical map.");
	  gv_sendfile("oogl/earth_coverage_h.oogl");
	  redraw_coverage_display();
	} else {
	  if (debug) error("texturemap sphere, dynamic mapping of this projection not supported.");
	  if (use_fancy_earth) {
	    gv_sendfile("oogl/earth_fancy_h.oogl");
	  } else {
	    gv_sendfile("oogl/earth_h.oogl");
	  }
	  geomview_dynamic_coverage = FALSE;
	}
	create_earth();
      } else if (use_simple_earth) {
	/* static texture - blue and gold map */
	if (debug) error ("texturemap sphere, simple static Earth image.\n");
	gv_sendfile("oogl/earth_h.oogl");
	create_earth();
      } else if (use_fancy_earth) {
	/* static texture - detailed texturemap */
	if (debug) error ("texturemap sphere, detailed static Earth image.\n");
	if (!geomview_stream_textures) {
	  /* 1.9.0 supports streaming and CYLINDRICAL/RECTANGULAR */
	  error("Detailed Earth map requires later Geomview. Please upgrade.");
	}
	gv_sendfile("oogl/earth_fancy_h.oogl");
	create_earth();
      } else {
	if (debug) error ("texturemap sphere, do not draw detailed Earth.\n");
	if (geomview_dynamic_coverage) {
	  if (debug) error ("texturemap sphere, do not draw detailed Earth.\n");
	  gv_create_geom("Central_Body", "central_t", "earth_h");
	} else {
	  if (debug) error ("basic sphere, no detailed Earth.\n");
	  gv_create_geom("Central_Body", "central_t", "unit_sphere_h");
        }
      }
    } else if (earth_on_flag) {
      /* no texturing */
      create_earth();
    }
    gv_stop();
  }

  earth_geom_exists = TRUE;

  return status;
}

/*
 * earth_off_cmd
 *
 * Destroy the earth sphere
 */
char *
earth_off_cmd(int argc, char *argv[])
{
  char *status = EMPTY_str;

  if (!earth_on_flag) return status;

  earth_on_flag = FALSE;

  if (use_fancy_earth) {
      /* we nest in gv_start/stop to hide temporary cosmetic changes -
       * this is the same idea used when loading tcl scripts
       * to prevent incremental per-new-satellite drawing updates.
       */
      if (geomview_module) gv_start();
      fancy_off_cmd(argc, argv);
      fancy_on_cmd(argc, argv);
      if (geomview_module) gv_stop();
  } else if (use_simple_earth) {
      if (geomview_module) gv_start();
      simple_off_cmd(argc, argv);
      simple_on_cmd(argc, argv);
      if (geomview_module) gv_stop();
  } else {
     /* just destroy sphere */
     earth_delete_geom();
  }

  return status;
}

/*
 * simple_on_cmd
 *
 * Creates a simple model of the earth by using vector
 * or loading texture-map file.
 */
char *
simple_on_cmd(int argc, char *argv[])
{
  char *status = EMPTY_str;

  if (use_simple_earth) return status;

  use_simple_earth = TRUE;

  if (geomview_module) gv_start();

  if (!earth_vectors_loaded) {
     earth_vectors_loaded = gv_sendfile("oogl/earth_vect_h.oogl");
  }

  if (earth_on_flag) {
    status = earth_off_cmd(argc, argv);
    status = earth_on_cmd(argc, argv);
  } else {
    /* clear sphere */
    if (debug) error("clear sphere, invisible Earth body.");
    gv_sendfile("oogl/earth_vect_h.oogl");
    gv_create_geom("Central_Body", "central_t", "earth_vect_clear_h");
    earth_geom_exists = TRUE;
  }
  if (geomview_module) gv_stop();

  return status;
}


/*
 * simple_off_cmd
 *
 * Returns to a clean sphere to model the central body
 */
char *
simple_off_cmd(int argc, char *argv[])
{
  char *status = EMPTY_str;

  if (!use_simple_earth) return status;

  use_simple_earth = FALSE;

  redraw_earth(argc, argv);

  return status;
}

/*
 * fancy_on_cmd
 *
 * Creates a fancy model of the earth by using vector
 * or loading texture-map file.
 */
char *
fancy_on_cmd(int argc, char *argv[])
{
  char *status = EMPTY_str;

  if (use_fancy_earth) return status;

  if (!geomview_detailed_texturemap) {
    error("Detailed texturemap file has not been downloaded.");
    return status;
  }

  use_fancy_earth = TRUE;

  if (geomview_module) gv_start();

  if (!earth_vectors_loaded) {
     earth_vectors_loaded = gv_sendfile("oogl/earth_vect_h.oogl");
  }

  if (earth_on_flag) {
    status = earth_off_cmd(argc, argv);
    status = earth_on_cmd(argc, argv);
  } else {
    /* clear sphere */
    if (debug) error("clear sphere, invisible Earth body.");
    gv_sendfile("oogl/earth_vect_h.oogl");
    gv_create_geom("Central_Body", "central_t", "earth_vect_clear_h");
    earth_geom_exists = TRUE;
  }
  if (geomview_module) gv_stop();

  return status;
}


/*
 * fancy_off_cmd
 *
 * Returns to a clean sphere to model the central body
 */
char *
fancy_off_cmd(int argc, char *argv[])
{
  char *status = EMPTY_str;

  if (!use_fancy_earth) return status;

  use_fancy_earth = FALSE;

  redraw_earth(argc, argv);

  return status;
}

/*
 * texture_on
 *
 * Use texture mapping for fancy earth model
 */
char *
texture_on_cmd(int argc, char *argv[])
{
  char *status = EMPTY_str;

  if (texture_flag) return status;

  texture_flag = TRUE;

  redraw_earth(argc, argv);

  return status;
}

/*
 * texture_off_cmd
 *
 * Use vect model for fancy earth model
 */
char *
texture_off_cmd(int argc, char *argv[])
{
  char *status = EMPTY_str;

  if (!texture_flag) return status;

  texture_flag = FALSE;

  redraw_earth(argc, argv);

  return status;
}

/*
 * geomview_dynamic_texture_on
 */
char *
geomview_dynamic_texture_on_cmd(int argc, char *argv[])
{
  char *status = EMPTY_str;

  if (geomview_dynamic_texture_flag) return status;

  if (!geomview_dynamic_coverage) {
    geomview_dynamic_coverage = coverage_dynamic_initialised();
  }
  if (geomview_dynamic_coverage) {
    geomview_dynamic_texture_flag = TRUE;
  }

  if (texture_flag) {
    redraw_earth(argc, argv);
  }

  return status;
}

/*
 * geomview_dynamic_texture_off
 */
char *
geomview_dynamic_texture_off_cmd(int argc, char *argv[])
{
  char *status = EMPTY_str;

  if (!geomview_dynamic_texture_flag) return status;

  geomview_dynamic_texture_flag = FALSE;

  if (texture_flag) {
    redraw_earth(argc, argv);
  }

  return status;
}

/*
 * earth_place
 *
 * rotates the earth to correct position for time t
 */
void
earth_place(double t, const CentralBody * pcb)
{
  double theta, ct, st;

  theta = t * pcb->rotation_rate;

  ct = cos(theta);
  st = sin(theta);

  /*
   * update elements of an existing identity matrix
   */
  m[0][0] = ct;
  m[0][1] = st;
  m[1][0] = -st;
  m[1][1] = ct;

  gv_transform("central_t", m);

}
