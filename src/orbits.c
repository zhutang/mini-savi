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
 * orbits.c
 *
 * routines to display orbits
 *
 * $Id: orbits.c,v 1.27 2017/01/02 06:11:32 lloydwood Exp $
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

static unsigned int orbits_on_flag = FALSE;
static unsigned int orbits_geom_exists = FALSE;
static unsigned int orbits_geom_needs_updating = FALSE;

static char name[] = "orbit_          ";

static void
orbit_write_geom(const Satellite s, const Constellation * pconstellation);

/*
 * orbits_on_cmd()
 *
 * Routine to display orbits
 */
char *
orbits_on_cmd(int argc, char *argv[])
{
  Constellation *pconstellation = get_constellation();
  Satellite_list sl = pconstellation->satellites;

  if (orbits_on_flag)
    return EMPTY_str;

  orbits_on_flag = TRUE;
  orbits_geom_needs_updating = TRUE;

  gv_start();
  while (sl) {
    orbit_write_geom(sl->s, pconstellation);
    sl = sl->next;
  }
  write_orbits_geom(pconstellation);
  gv_stop();

  return EMPTY_str;
}

/*
 * orbits_off_cmd()
 *
 * Routine to turn off display of orbits
 */
char *
orbits_off_cmd(int argc, char *argv[])
{
  Satellite_list sl = get_constellation()->satellites;

  if (!orbits_on_flag)
    return EMPTY_str;

  gv_start();
  orbits_gv_delete();
  while (sl) {
    orbit_delete(sl->s);
    sl = sl->next;
  }
  orbits_geom_needs_updating = FALSE;
  orbits_on_flag = FALSE;
  gv_stop();

  return EMPTY_str;
}

/*
 * orbits_gv_delete
 *
 * routine to delete geomview orbits object
 */
void
orbits_gv_delete()
{
  if (orbits_geom_exists) {
    gv_delete_geom("Orbits");
    orbits_geom_exists = FALSE;
  }

}

char *
orbits_set_cmd(int argc, char *argv[])
{
  Constellation *pconstellation = get_constellation();
  Satellite s;

  int n = atoi(argv[2]);
  unsigned int flag = atoi(argv[3]);

  s = constellation_search(pconstellation, n);
  if (s) {
    if (flag)
      s->can_display_orbit = 1;
    else
      s->can_display_orbit = 0;
    orbits_geom_needs_updating = TRUE;
  }

  return EMPTY_str;
}


/*
 * orbit_write_geom
 *
 * Write orbit geom for a satellite
 */
static void
orbit_write_geom(const Satellite s, const Constellation * pconstellation)
{
  unsigned int i, segments_drawn, segment_endpoints;
  double t, ellipse, segment_time;
  CartesianCoordinates u;

  CentralBody *pcb = pconstellation->pcb;
  double scale = pcb->radius;

  ellipse = s->oe_t.e;

  segments_drawn = GV_ORBIT_SEGMENTS / (1-ellipse);
  segment_time = s->period / segments_drawn;
  segment_endpoints = segments_drawn + 1;

  /* start of gv orbit description */
  fprintf(gv_out, "(read geometry {define orbit_%d {VECT\n1 %d 1\n%d\n1\n\n",
	  s->id, segment_endpoints, segment_endpoints);

  for (i = 0; i < segment_endpoints; i++) {
    t = i * segment_time;
    oe_time_to_geocentric(&u, t, &(s->oe_t), pcb);

    fprintf(gv_out, "%f %f %f\n", u.x / scale, u.y / scale, u.z / scale);
  }

  /* These four numbers specify the color (RGBA). */
  if (s->tag)
    gv_send("1 1 0 1} })\n");
  else
    gv_send("1 0 0 1} })\n");
}

/*
 * orbit_display
 *
 * Takes a satellite and displays its orbit
 */
void
orbit_display(Satellite s, const Constellation * pconstellation)
{
  if (!orbits_on_flag)
    return;

  orbits_geom_needs_updating = TRUE;

  gv_start();
  orbit_write_geom(s, pconstellation);
  gv_stop();
}

/*
 * orbits_relocate
 *
 * Called to update the orbits when the time has changed
 */
void
orbits_relocate(const Constellation * pconstellation)
{
  Satellite_list sl = pconstellation->satellites;

  if (!orbits_on_flag)
    return;

  while (sl) {
    if (sl->s->can_display_orbit)
      orbit_write_geom(sl->s, pconstellation);
    sl = sl->next;
  }
}


/*
 * orbit_delete
 *
 * Takes a satellite and deletes its orbit
 * ONLY if orbits_on_flag == TRUE
 */
void
orbit_delete(Satellite s)
{
  if (!orbits_on_flag)
    return;

  orbits_geom_needs_updating = TRUE;

  gv_start();
  sprintf(name + 6, "%-10d", s->id);
  gv_delete_handle(name);
  gv_stop();
}



void
write_orbits_geom(const Constellation * pconstellation)
{
  Satellite_list sl = pconstellation->satellites;

  if (orbits_on_flag && orbits_geom_needs_updating) {
    /* write out new list of all satellites */
    gv_send("(geometry Orbits {LIST\n");
    while (sl) {
      if (sl->s->can_display_orbit) {
	fprintf(gv_out, ":orbit_%d\n", sl->s->id);
      }
      sl = sl->next;
    }
    gv_send("})\n");

    orbits_geom_needs_updating = FALSE;
    orbits_geom_exists = TRUE;
  }
}
