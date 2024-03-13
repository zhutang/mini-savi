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
 *
 * sun.c
 *
 * Routines for creating fancy sun model
 *
 * $Id: sun.c,v 1.17 2017/01/02 06:11:32 lloydwood Exp $
 */
#include <stdio.h>
#include <math.h>

#include "gv_utils.h"
#include "utils.h"
#include "constants.h"
#include "globals.h"
#include "savi.h"
#include "sats.h"

/*
 * sun_on_cmd
 *
 * Adjusts lighting for day/night shadows
 * and draws line from central body towards Sun in Geomview.
 *
 */
char *
sun_on_cmd(int argc, char *argv[])
{
  if (sun_flag) return EMPTY_str;

  sun_flag = TRUE;

  if (geomview_module) {
    gv_start();
    sun_place(current_time(), (get_constellation())->pcb);
    if (footprints_flag) {
      footprints_off_cmd(argc, argv);
      footprints_on_cmd(argc, argv);
    }
    gv_stop();
  }

  return EMPTY_str;
}

/*
 * sun_off_cmd
 *
 * Resets to uniform lighting.
 */
char *
sun_off_cmd(int argc, char *argv[])
{
  if (!sun_flag) return EMPTY_str;

  sun_flag = FALSE;

  if (geomview_module) {
    gv_start();
    if (geomview_sun_lighting) gv_sendfile(AMBIENT_LIGHTING);
    gv_delete_geom("Sun");
    if (footprints_flag) {
      footprints_off_cmd(argc, argv);
      footprints_on_cmd(argc, argv);
    }
    gv_stop();
  }

  return EMPTY_str;
}

/*
 * sun_place
 *
 * rotates the sun to correct position for time t
 *
 * Time 0 is determined to be an equinox with the
 * 0 longitude turned away from the Sun
 *
 */
void
sun_place(double t, const CentralBody *pcb)
{
  char buf[LENGTH_STRING_BUFFER];
  double theta, phi, x, y, z;

  if (!sun_flag) return;

  /* theta is the earth in the sun's orbit */
  theta = t / pcb->orbital_period * TWOPI + PI;

  /* phi is the angle above the equatorial plane */
  phi = pcb->tilt * sin(theta) * DEG_TO_RAD;

  /* position the light source at x, y, z */
  x = cos(theta)*cos(phi);
  y = sin(theta)*cos(phi);
  z = sin(phi);

  gv_begin();

  if (geomview_sun_lighting) {
    gv_send("(merge-baseap appearance { lighting { ambient .7 .7 .7 replacelights light { ");
    gv_send("color 1 1 1 position ");
    sprintf(buf,"%f %f %f 0", x, y, z);
    gv_send(buf);
    gv_send(" location global } } } )");
  }

  gv_send("(geometry Sun {VECT 1 2 1 2 1 ");
  sprintf(buf,"%f %f %f", x, y, z);
  gv_send(buf);
  sprintf(buf, " %f %f %f",2*x, 2*y, 2*z);
  gv_send(buf);
  gv_send(" 1 1 0 1 } )");

  gv_end();
}
