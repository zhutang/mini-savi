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
 * Satellite.c
 *
 * Routines for Satellite object
 *
 * $Id: Satellite.c,v 1.21 2017/01/02 06:11:32 lloydwood Exp $
 */

#include <string.h>

#include "constants.h"
#include "orbit_utils.h"
#include "stdlib.h"

#include "Satellite.h"

/*
 * satellite_create
 *
 * Pass unique id for new satellite plus orbital elements.
 * Returns a Satellite object (which has been positioned at t=0).
 */
Satellite satellite_create(int id, OrbitalElements * poe, CentralBody * pcb)
{
  Satellite s = (Satellite) calloc(1, sizeof(struct satellite));

  s->id = id;
  s->oe = *poe;
  s->tag = FALSE;

  s->gs_into_view = 0;
  s->can_display_satellite = 1;
  s->can_display_orbit = 1;
  s->can_display_coverage = 1;

  /* set name to default blank */
  s->name[0] = 0;

  /* give it a valid position; choose time 0 */
  satellite_compute_position(s, 0.0, pcb);

  /* precompute some quantities */
  satellite_oe_edited(s, pcb);

  return s;
}

/*
 * satellite_name
 *
 * Set name string on satellite, using unique id assigned by _create.
 */
int satellite_name(Satellite s, char * name)
{
  unsigned int n;

  if (!name) return FALSE;

  n = strlen(name);

  if (n < SATELLITE_NAME_LENGTH) {
    strcpy((char *)&s->name, name);
  } else {
    error("satellite name too long. Ignored.");
  }

  return TRUE;
}


/*
 * satellite_oe_edited
 *
 * Call this after time 0 orbital elements (s->oe) have been changed.
 * It updates some precomputed quantities.
 */
void
satellite_oe_edited(Satellite s, CentralBody * pcb)
{

  /* determine classical (Keplerian) period */
  s->period = oe_to_period(&(s->oe), pcb);
}


/*
 * satellite_destroy
 *
 * Frees memory for a Satellite object
 */
void
satellite_destroy(Satellite s)
{
  free((char *) s);
}


/*
 * satellite_compute_position
 *
 * Updates the orbital elements, cartesian and spherical coordinates
 *  of a satellite for time t
 */
void
satellite_compute_position(Satellite s, const double t, CentralBody * pcb)
{
  oe_time_to_oe(&(s->oe_t), &(s->oe), t, pcb);
  oe_to_geocentric(&(s->x_C), &(s->oe_t), pcb);
  cartesian_to_spherical(&(s->x_S), &(s->x_C));
  s->t = t;
  spherical_to_transform(s->transform, &(s->x_S), pcb->radius);
}


/*
 * satellite_set_tag
 *
 * Sets the tag field for the satellite
 */
void
satellite_set_tag(Satellite s, int tag)
{
  s->tag = tag;
}


/*
 * satellite_get_tag
 *
 * Returns the tag field for the satellite
 */
int
satellite_get_tag(Satellite s)
{
  return s->tag;
}


/*
 * constellation_new_id
 *
 * returns an integer identifier which is not already used in
 * the specified constellation
 */
int
constellation_new_id(const Constellation * pconstellation)
{
  int n = 0;
  Satellite_list sl = pconstellation->satellites;

  while (sl && (sl->s->id == n)) {
    sl = sl->next;
    n++;
  }
  return n;
}

/*
 * constellation_add
 *
 * adds a satellite to a constellation, modifying
 * the satellite id to be the lowest possible one
 */
int
constellation_add(Constellation * pconstellation, Satellite s)
{
  int n = 0;
  Satellite_list newl;
  Satellite_list *psl = &(pconstellation->satellites);

  /* find id and empty position number */
  while (*psl && ((*psl)->s->id == n)) {
    psl = &((*psl)->next);
    n++;
  }

  /* set id on satellite */
  s->id = n;

  /* add to satellite list */
  newl = (Satellite_list) calloc(1, sizeof(struct satellite_list));
  newl->s = s;
  newl->next = *psl;
  (*psl) = newl;

  /* increment counters */
  (pconstellation->n_sats)++;
  if (s->can_display_satellite)
    (pconstellation->n_view_sats)++;
  if (s->tag)
    (pconstellation->n_tag_sats)++;

  return n;
}

/*
 * constellation_search(Constellation *pconstellation, int id)
 *
 * Returns the satellite with a specified id, else NULL.
 *
 */
Satellite constellation_search(const Constellation * pconstellation, int id)
{
  Satellite_list sl = pconstellation->satellites;

  while (sl && (id != sl->s->id))
    sl = sl->next;

  return (sl ? sl->s : NULL);
}

/*
 * constellation_compute_positions
 *
 * Computes the spherical coordinates position of each satellite in the constellation
 * for some specified time t.
 */
void
constellation_compute_positions(Constellation * pconstellation, double t)
{
  Satellite_list sl = pconstellation->satellites;

  while (sl) {
    satellite_compute_position(sl->s, t, pconstellation->pcb);
    sl = sl->next;
  }
}

/*
 * constellation_destroy_element
 *
 * Removes a satellite from a constellation based on its id
 * and destroys it.
 */
void
constellation_destroy_element(Constellation * pconstellation, int id)
{
  Satellite_list *slp = &(pconstellation->satellites);
  Satellite_list old = NULL;

  while (*slp && ((*slp)->s->id != id))
    slp = &((*slp)->next);

  if (*slp) {
    /* remove from list */
    old = *slp;
    *slp = old->next;

    /* decrement counters */
    (pconstellation->n_sats)--;
    if (old->s->tag)
      (pconstellation->n_tag_sats)--;
    if (old->s->can_display_satellite)
      (pconstellation->n_view_sats)--;

    /* release memory */
    satellite_destroy(old->s);
    free((char *) old);

  }
}
