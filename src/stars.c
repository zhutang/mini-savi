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
 * stars.c
 *
 * Routines for creating star field
 *
 * $Id: stars.c,v 1.16 2017/01/02 06:11:32 lloydwood Exp $
 */
#include <stdio.h>
#include <math.h>

#include "gv_utils.h"
#include "utils.h"
#include "constants.h"
#include "savi.h"
#include "globals.h"

void stars_delete(void);

static unsigned int stars_loaded = FALSE;
static unsigned int stars_geom_exists = FALSE;

/*
 * stars_on
 *
 * Creates a star field by loading file stars.oogl
 */
char *
stars_on_cmd(int argc, char *argv[])
{
  gv_start();
  if (!stars_loaded) {
    stars_loaded = gv_sendfile("oogl/stars_h.oogl");
  }
  if (stars_loaded) {
    gv_create_geomh("Stars", "stars_h");
    stars_geom_exists = TRUE;
  }
  gv_stop();
  return EMPTY_str;
}

/*
 * stars_off
 *
 * turns the stars off again
 */
char *
stars_off_cmd(int argc, char *argv[])
{
  stars_delete();
  return EMPTY_str;
}

/*
 * stars_delete
 *
 * Delete the stars from geomview
 */
void
stars_delete()
{
  if (!stars_geom_exists) return;

  gv_start();
  gv_delete_geom("Stars");
  gv_stop();
  stars_geom_exists = FALSE;

}
