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
 * cones.c
 *
 * routines for cones in Geomview
 *
 * $Id: cones.c,v 1.41 2017/01/02 06:11:32 lloydwood Exp $
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

static unsigned int cones_on_flag = FALSE;
static unsigned int cones_geom_exists = FALSE;
static unsigned int cones_geom_needs_updating = FALSE;
static int cones_angle_used = 0;

static char name[] = "cone_          ";
static double t[GV_FOOTPRINT_EDGE_SEGMENTS];

static void
cone_size(double *radius, double *height, const Satellite s, const
	  Constellation * pconstellation);
static void
cone_write_geom(const Satellite s, const Constellation * pconstellation);
static void
cone_gv_delete(const Satellite s);

/*
 * cones_on_cmd()
 *
 * Routine to display cones
 */
char *
cones_on_cmd(int argc, char *argv[])
{
  Constellation *pconstellation;
  Satellite_list sl;
  Satellite ps;

  if (cones_on_flag)
    return EMPTY_str;

  pconstellation = get_constellation();
  sl = pconstellation->satellites;

  cones_on_flag = TRUE;
  cones_geom_needs_updating = TRUE;

  transforms_needed |= (1 << CONES);

  gv_start();
  while (sl) {
    ps = sl->s;
    gv_trans_create(ps);
    cone_write_geom(ps, pconstellation);
    sl = sl->next;
  }
  write_cones_geom(pconstellation);
  gv_stop();

  /* remember what angle was used */
  cones_angle_used = coverage_angle;

  return EMPTY_str;
}

/*
 * cones_off_cmd()
 *
 * Routine to turn off display of cones
 */
char *
cones_off_cmd(int argc, char *argv[])
{
  Satellite_list sl;

  if (!cones_on_flag)
    return EMPTY_str;

  sl = get_constellation()->satellites;

  transforms_needed &= ~(1 << CONES);

  gv_start();
  cones_gv_delete();
  while (sl) {
    cone_gv_delete(sl->s);
    sl = sl->next;
  }
  cones_geom_needs_updating = FALSE;
  cones_on_flag = FALSE;
  gv_stop();

  return EMPTY_str;
}


/*
 * cones_rebuild()
 *
 * Routine to resize all the cones currently built
 */
void
cones_rebuild()
{
  Constellation *pconstellation;
  Satellite_list sl;

  if (!cones_on_flag)
    return;

  pconstellation = get_constellation();
  sl = pconstellation->satellites;

  gv_start();
  while (sl) {
    cone_write_geom(sl->s, pconstellation);
    sl = sl->next;
  }
  gv_stop();

  /* remember what angle was used */
  cones_angle_used = coverage_angle;

  cones_geom_needs_updating = FALSE;
}

/*
 * cones_gv_update
 *
 * get ready to update drawing in geomview.
 */
void
cones_gv_update()
{
  cones_geom_needs_updating = TRUE;
}


/*
 * cones_gv_delete
 *
 * routine to delete geomview cones object
 */
void
cones_gv_delete()
{
  if (cones_geom_exists) {

    gv_delete_geom("Cones");

    cones_geom_exists = FALSE;
  }
}


/*
 * cones_relocate
 *
 * Called to update the cone when the satellites have all been moved
 *
 */
void
cones_relocate(const Constellation * pconstellation)
{
  Satellite_list sl;
  Satellite ps;
  unsigned int angle_changed;

  if (!cones_on_flag)
    return;

  sl = pconstellation->satellites;
  angle_changed = (cones_angle_used != coverage_angle);

  gv_start();
  while (sl) {
    ps = sl->s;
    if (angle_changed || (ps->oe.e > 0)) {
      cone_write_geom(ps, pconstellation);
    }
    sl = sl->next;
  }
  gv_stop();

  /* remember what angle was used */
  cones_angle_used = coverage_angle;
}

/*
 * cone_size
 *
 * computes radius and height of cone for given satellite
 *
 */
static void
cone_size(double *radius, double *height, const Satellite s, const
	  Constellation * pconstellation)
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
  c = cos(theta);
  si = sin(theta);
  r = s->x_S.r / pcb->radius;

  if (MASK_ELEVATION == get_coverage_type()) {
    temp = sqrt(r * r - c * c);
    *radius = c * (temp - si) / r;
    *height = temp * (temp - si) / r;
  } else {
    if (r * si < 1) {
      /* cone intersects earth */
      if (si == 0) {
	/* trivial case - theta = 0 */
	*radius = 0.0;
	*height = r - 1.0;
      } else {
	*radius = sin(asin(r * si) - theta);
	*height = (*radius) * c / si;
      }
    } else {
      /* cone does not intersect earth */
      *radius = r * si * c;
      *height = r * c * c;
    }
  }
}

/*
 * cone_write_geom
 *
 * Write cone geom for a satellite
 */
static void
cone_write_geom(const Satellite s, const Constellation * pconstellation)
{
  unsigned int i;

  for (i = 0; i < GV_FOOTPRINT_EDGE_SEGMENTS - 1; i++) {
    t[i] = 0.0;
  }
  t[GV_FOOTPRINT_EDGE_SEGMENTS - 1] = 1.0;

  /*
     compute scaling of cone -
     a rigid body rotation plus a translation
     will be applied to the cone to place it so
     that (0,0,0) will be mapped to the position of the satellite
     and (0,0,-1) will point to the origin
   */
  cone_size(t, t + 10, s, pconstellation);
  t[5] = t[0];

  /* start of gv cone description */
  fprintf(gv_out,
	  "(read geometry {define cone_%d { INST transform { ", s->id);
  for (i = 0; i < GV_FOOTPRINT_EDGE_SEGMENTS; i++) {
    fprintf(gv_out, "%g ", t[i]);
  }
  fprintf(gv_out, "} geom: cone_h } } )\n");
}


/*
 * cone_display
 *
 * Takes a satellite and displays its cone (if turned on).
 */
void
cone_display(Satellite s, const Constellation * pconstellation)
{
  if (!cones_on_flag)
    return;
  cones_geom_needs_updating = TRUE;

  gv_start();
  cone_write_geom(s, pconstellation);
  gv_stop();
}


/*
 * cone_gv_delete
 *
 * Takes a satellite and deletes its cone
 */
static void
cone_gv_delete(Satellite s)
{
  sprintf(name + 5, "%-10d", s->id);
  gv_delete_handle(name);
}


/*
 * cone_delete
 *
 * Takes a satellite and deletes its cone
 * ONLY if cones_on_flag == TRUE
 */
void
cone_delete(Satellite s)
{

  if (!cones_on_flag)
    return;

  gv_start();
  cone_gv_delete(s);
  gv_stop();

  cones_geom_needs_updating = TRUE;
}



void
write_cones_geom(const Constellation * pconstellation)
{
  Satellite ps;
  Satellite_list sl = pconstellation->satellites;
  int sid;

  if (cones_on_flag && cones_geom_needs_updating) {
    /* write out new list of all satellites */
    gv_send("(geometry Cones {LIST\n");

    /* always skip sunlight cone */
    sl = sl->next;

    while (sl) {
      ps = sl->s;
      if (ps->can_display_coverage) {
	sid = ps->id;
	fprintf(gv_out, "{ INST transform:trans_%d geom:cone_%d}\n", sid,
		sid);
      }
      sl = sl->next;
    }
    gv_send("})\n");

    cones_geom_exists = TRUE;
    cones_geom_needs_updating = FALSE;
  }
}
