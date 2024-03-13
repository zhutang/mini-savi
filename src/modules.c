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
 * modules.c
 *
 *
 * Eventually additional features will be all installed here
 * and the code will loop over elements at appropriate times.
 * Right now, most of the calls to the modules are sprinkled
 * throughout the code :(
 *
 * $Id: modules.c,v 1.15 2017/01/02 06:11:32 lloydwood Exp $
 */

#include "Satellite.h"
#include "savi.h"

ViewModule view_modules[] = {

  {NULL, NULL, NULL, NULL,
   NULL, earth_off_cmd, NULL},

  {NULL, NULL, NULL, NULL,
   NULL, sun_off_cmd, NULL},

  {NULL, NULL, NULL, NULL,
   NULL, axes_off_cmd, NULL},

  {NULL, NULL, NULL, NULL,
   NULL, plane_off_cmd, NULL},

  {NULL, NULL, NULL, NULL,
   NULL, stars_off_cmd, NULL},

  {NULL, NULL, NULL, NULL,
   NULL, logo_off_cmd, NULL},

  {write_satellites_geom, NULL, satellite_display, satellites_relocate,
   satellite_delete, satellites_off_cmd, satellites_gv_delete},

  {write_orbits_geom, NULL, orbit_display, orbits_relocate,
   orbit_delete, orbits_off_cmd, orbits_gv_delete},

  {write_cones_geom, cones_rebuild, cone_display, cones_relocate,
   cone_delete, cones_off_cmd, cones_gv_delete},

  {write_footprints_geom, footprints_rebuild, footprint_display,
   footprints_relocate,
   footprint_delete, footprints_off_cmd, footprints_gv_delete}

};

const int N_VIEW_MODULES = sizeof(view_modules) / sizeof(ViewModule);

CompModule comp_modules[] = {
  {NULL}
  ,
};

const int N_COMP_MODULES = sizeof(comp_modules) / sizeof(CompModule);
