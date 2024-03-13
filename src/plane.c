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
 * plane.c
 *
 * $Id: plane.c,v 1.17 2017/01/02 06:11:32 lloydwood Exp $
 */
#include <math.h>
#include <stdlib.h>

#include "constants.h"
#include "gv_utils.h"
#include "utils.h"
#include "savi.h"
#include "globals.h"

static unsigned int plane_geom_exists = FALSE;

/*
 * plane
 *
 * Routine to place plane at a particular angle
 * (will create plane geom if necessary).
 *
 * Expects i and Omega to be in degrees, since user interface
 * is that way.
 */
char *
plane_cmd(int argc, char *argv[])
{
  double i = atof(argv[2]);
  double Omega = atof(argv[3]);
  double m[4][4], ci, si, cO, sO;

  plane_flag = TRUE;

  if (!geomview_module)
    return EMPTY_str;

  identity(m);
  ci = cos(i * DEG_TO_RAD);
  si = sin(i * DEG_TO_RAD);
  cO = cos(Omega * DEG_TO_RAD);
  sO = sin(Omega * DEG_TO_RAD);
  m[0][0] = cO;
  m[0][1] = sO;
  m[1][0] = -ci * sO;
  m[1][1] = ci * cO;
  m[1][2] = si;
  m[2][0] = si * sO;
  m[2][1] = -si * cO;
  m[2][2] = ci;

  gv_start();
  gv_transform("plane_t", m);
  if (!plane_geom_exists) {
    gv_create_geom("Plane", "plane_t", "plane_h");
  }
  plane_geom_exists = TRUE;
  gv_stop();

  return EMPTY_str;
}

/*
 * plane_off
 *
 * Deletes plane from geomview
 */
char *
plane_off_cmd(int argc, char *argv[])
{
  if (!plane_flag)
    return EMPTY_str;

  if(geomview_module) {
    gv_start();
    gv_delete_geom("Plane");
    plane_geom_exists = FALSE;
    gv_stop();
  }

  plane_flag = FALSE;

  return EMPTY_str;
}
