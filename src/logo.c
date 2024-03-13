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
 * logo.c
 *
 * $Id: logo.c,v 1.18 2017/01/02 06:11:32 lloydwood Exp $
 */
#include <stdlib.h>
#include "gv_utils.h"
#include "constants.h"
#include "globals.h"
#include "savi.h"

static unsigned int logo_geom_exists = FALSE;

/*
 * logo_on_cmd
 *
 * Display axes in geomview
 */
char *
logo_on_cmd(int argc, char *argv[])
{
  if (logo_geom_exists)
    return EMPTY_str;
  logo_geom_exists = TRUE;

  gv_create_alienh("Logo", "logo_h");

  return EMPTY_str;
}

/*
 * logo_off_cmd
 *
 * Deletes logo in Geomview
 */
char *
logo_off_cmd(int argc, char *argv[])
{
  if (!logo_geom_exists)
    return EMPTY_str;
  logo_geom_exists = FALSE;

  gv_delete_geom("Logo");

  return EMPTY_str;
}
