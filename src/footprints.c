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
 * footprints.c
 *
 * routines for footprints in Geomview
 *
 * $Id: footprints.c,v 1.37 2017/01/02 06:11:32 lloydwood Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "globals.h"
#include "constants.h"
#include "gv_file.h"
#include "gv_utils.h"
#include "sats.h"
#include "orbit_utils.h"
#include "savi.h"

static unsigned int footprints_geom_exists = FALSE;
static unsigned int footprints_geom_needs_updating = FALSE;
static int footprints_angle_used = 0;

static char name[] = "footprint_          ";
static double t[GV_FOOTPRINT_EDGE_SEGMENTS];

static void
footprint_size(double *radius, double *height, const Satellite s, const
	       Constellation * pconstellation, int special);
static void
footprint_write_geom(const Satellite s, const Constellation * pconstellation, int special);
static void
footprint_gv_delete(const Satellite s);

/*
 * footprints_on_cmd()
 *
 * Routine to display footprints
 */
char *
footprints_on_cmd(int argc, char *argv[])
{
  Constellation *pconstellation;
  Satellite_list sl;
  Satellite ps;

  if (footprints_flag)
    return EMPTY_str;

  pconstellation = get_constellation();
  sl = pconstellation->satellites;

  footprints_flag = TRUE;
  footprints_geom_needs_updating = TRUE;

  transforms_needed |= (1 << FOOTPRINTS);

  ps = sl->s;

  gv_start();

  gv_trans_create(ps);
  footprint_write_geom(ps, pconstellation, TRUE);

  sl = sl->next;

  while (sl) {
    ps = sl->s;
    gv_trans_create(ps);
    footprint_write_geom(ps, pconstellation, FALSE);
    sl = sl->next;
  }
  write_footprints_geom(pconstellation);
  gv_stop();

  /* remember what angle was used */
  footprints_angle_used = coverage_angle;

  return EMPTY_str;
}

/*
 * footprints_off_cmd()
 *
 * Routine to turn off display of footprints
 */
char *
footprints_off_cmd(int argc, char *argv[])
{
  Satellite_list sl;

  if (!footprints_flag)
    return EMPTY_str;

  sl = get_constellation()->satellites;

  transforms_needed &= ~(1 << FOOTPRINTS);

  gv_start();
  footprints_gv_delete();
  while (sl) {
    footprint_gv_delete(sl->s);
    sl = sl->next;
  }
  footprints_geom_needs_updating = FALSE;
  footprints_flag = FALSE;
  gv_stop();

  return EMPTY_str;
}


/*
 * footprints_rebuild()
 *
 * Routine to resize all the footprints currently built
 */
void
footprints_rebuild()
{
  Constellation *pconstellation;
  Satellite_list sl;

  if (!footprints_flag)
    return;

  pconstellation = get_constellation();
  sl = pconstellation->satellites;

  /* do sunlight separately */
  footprint_write_geom(sl->s, pconstellation, TRUE);
  sl = sl->next;

  gv_start();
  while (sl) {
    footprint_write_geom(sl->s, pconstellation, FALSE);
    sl = sl->next;
  }
  gv_stop();

  /* remember what angle was used */
  footprints_angle_used = coverage_angle;

  footprints_geom_needs_updating = FALSE;
}


/*
 * footprints_gv_delete
 *
 * routine to delete geomview footprints object
 */
void
footprints_gv_delete()
{
  if (footprints_geom_exists) {

    gv_delete_geom("Footprints");

    footprints_geom_exists = FALSE;
  }
}


/*
 * footprints_relocate
 *
 * Called to update the footprint when the satellites have all been moved
 *
 */
void
footprints_relocate(const Constellation * pconstellation)
{
  Satellite_list sl;
  Satellite ps;
  unsigned int angle_changed;

  if (!footprints_flag)
    return;

  sl = pconstellation->satellites;
  angle_changed = (footprints_angle_used != coverage_angle);

  gv_start();

  /* skip sunlight - it should never change */
  ps = sl->s;
  if (angle_changed || (ps->oe.e > 0)) {
      footprint_write_geom(ps, pconstellation, TRUE);
  }
  sl = sl->next;

  while (sl) {
    ps = sl->s;
    if (angle_changed || (ps->oe.e > 0)) {
      footprint_write_geom(ps, pconstellation, FALSE);
    }
    sl = sl->next;
  }
  gv_stop();

  /* remember what angle was used */
  footprints_angle_used = coverage_angle;
}

/*
 * footprint_size
 *
 * computes radius and height of footprint for given satellite
 *
 */
static void
footprint_size(double *radius, double *height, const Satellite s, const
	       Constellation * pconstellation, int special)
{
  CentralBody *pcb = pconstellation->pcb;
  double a, r, theta, c, si, temp;

  a = s->x_S.r - pcb->radius;
  if (a <= min_transmit_altitude) {
    *radius = 0;
    *height = 0;
    return;
  }

  /* coverage_angle is global variable (in degrees) */
  theta = coverage_angle * DEG_TO_RAD;
  if (special) {
    if (MASK_ELEVATION == get_coverage_type()) {
      theta = 0;
    } else {
      theta = HALFPI;
    }
  }
  c = cos(theta);
  si = sin(theta);
  r = s->x_S.r / pcb->radius;

  if (MASK_ELEVATION == get_coverage_type()) {
    temp = sqrt(r * r - c * c);
    *radius = c * (temp - si) / r;
    *height = temp * (temp - si) / r;
  } else {
    /* SATELLITE_CONE */
    if (r * si < 1) {
      /* coverage cone intersects earth */
      if (si == 0) {
	/* trivial case - theta = 0 */
	*radius = 0.0;
	*height = r - 1.0;
      } else {
	*radius = sin(asin(r * si) - theta);
	*height = (*radius) * c / si;
      }
    } else {
      /* coverage cone does not intersect earth */
      *radius = sqrt(1 - 1 / (r * r));
      *height = r - 1 / r;
    }
  }
}

/*
 * footprint_write_geom
 *
 * Write footprint geom for a satellite
 */
static void
footprint_write_geom(const Satellite s, const Constellation * pconstellation, int special)
{
  unsigned int i;

  if (special && !sun_flag) return;

  for (i = 0; i < GV_FOOTPRINT_EDGE_SEGMENTS - 1; i++) {
    t[i] = 0.0;
  }
  t[GV_FOOTPRINT_EDGE_SEGMENTS - 1] = 1.0;

  /*
     compute scaling of footprint -
     a rigid body rotation plus a translation
     will be applied to the footprint to place it so
     that (0,0,0) will be mapped to the position of the satellite
     and (0,0,-1) will point to the origin
   */
  footprint_size(t, t + 10, s, pconstellation, special);
  t[5] = t[0];

  /* start of gv footprint description */
  fprintf(gv_out,
          "(read geometry {define footprint_%d { INST transform { ", s->id);
  for (i = 0; i < GV_FOOTPRINT_EDGE_SEGMENTS; i++) {
    fprintf(gv_out, "%g ", t[i]);
  }
  if (special) {
    fprintf(gv_out, "} geom: terminator_h } } )\n");
  } else {
    fprintf(gv_out, "} geom: footprint_h } } )\n");
  }
}

/*
 * footprint_display
 *
 * Takes a satellite and displays its footprint.
 */
void
footprint_display(Satellite s, const Constellation * pconstellation)
{
  if (!footprints_flag)
    return;
  footprints_geom_needs_updating = TRUE;

  gv_start();
  footprint_write_geom(s, pconstellation, FALSE); /* what if it's sun? */
  gv_stop();
}

/*
 * footprints_gv_update
 *
 * get ready to update drawing in geomview.
 */
void
footprints_gv_update()
{
  footprints_geom_needs_updating = TRUE;
}

/*
 * footprint_gv_delete
 *
 * Takes a satellite and deletes its footprint
 */
static void
footprint_gv_delete(Satellite s)
{
  sprintf(name + 10, "%-10d", s->id);
  gv_delete_handle(name);
}



/*
 * footprint_delete
 *
 * Takes a satellite and deletes its footprint
 * ONLY if footprints_flag == TRUE
 */
void
footprint_delete(Satellite s)
{

  if (!footprints_flag)
    return;

  footprints_geom_needs_updating = TRUE;

  gv_start();
  footprint_gv_delete(s);
  gv_stop();
}



void
write_footprints_geom(const Constellation * pconstellation)
{
  Satellite ps;
  Satellite_list sl = pconstellation->satellites;
  int sid;

  if (footprints_flag && footprints_geom_needs_updating) {
    /* write out new list of all satellites */
    gv_send("(geometry Footprints {LIST\n");

    /* skip sunlight cone if turned off */
    if (!sun_flag)sl = sl->next;

    while (sl) {
      ps = sl->s;
      if (ps->can_display_coverage) {
	sid = ps->id;
	fprintf(gv_out,
	        "{ INST transform:trans_%d geom:footprint_%d }\n", sid,
	        sid);
      }
      sl = sl->next;
    }
    gv_send("})\n");

    footprints_geom_exists = TRUE;
    footprints_geom_needs_updating = FALSE;
  }
}
