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
 * axes.c
 *
 * $Id: axes.c,v 1.21 2017/01/02 06:11:32 lloydwood Exp $
 */
#include <stdlib.h>

#include "constants.h"
#include "gv_utils.h"
#include "globals.h"
#include "savi.h"


static unsigned int axes_geom_exists = FALSE;

/*
 * axes_on_cmd
 *
 * Display axes in geomview
 */
char *
axes_on_cmd(int argc, char *argv[])
{
  if (axes_geom_exists)
    return EMPTY_str;
  axes_geom_exists = TRUE;

  gv_start();
  gv_create_geomh("Axes", "axes_h");
  gv_stop();

  return EMPTY_str;
}

/*
 * axes_off_cmd
 *
 * Deletes axes in Geomview
 */
char *
axes_off_cmd(int argc, char *argv[])
{
  if (!axes_geom_exists)
    return EMPTY_str;
  axes_geom_exists = FALSE;

  gv_start();
  gv_delete_geom("Axes");
  gv_stop();

  return EMPTY_str;
}
