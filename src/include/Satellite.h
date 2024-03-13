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
 * Satellite.h
 *
 * $Id: Satellite.h,v 1.21 2017/01/02 06:11:32 lloydwood Exp $
 */

#ifndef _SATELLITE_H_
#define _SATELLITE_H_

#include "orbit_utils.h"
/*
 * define the data structure of a single satellite
 *
 */

/* 24 in TLE definition, plus zero terminator */
#define SATELLITE_NAME_LENGTH 25

typedef struct satellite {
  OrbitalElements oe;                    /* classical orbital elements  */
  OrbitalElements oe_t;                  /* orbital elements at time t  */
  CartesianCoordinates x_C;              /* position in geocentric      */
  SphericalCoordinates x_S;              /*   coordinate systems at t   */
  double t;                              /* time                        */
  double period;                         /* classical period            */
  double transform[4][4];                /* transform for use in gv     */
  float gs_into_view;                    /* time sat into view on ground*/
  float gs_last_into_view;		 /* previous encounter time     */
  float gs_last_pass;			 /* time sat last pass on ground*/
  unsigned int can_display_satellite:1;  /*   satellite display flag    */
  unsigned int can_display_orbit:1;      /*   orbit display flag        */
  unsigned int can_display_coverage:1;   /*   coverage display flag     */
  int tag;                               /* used for marking satellites */
  int id;                                /* identifier of satellite     */
  char name[SATELLITE_NAME_LENGTH];      /*      name identifier        */
} *Satellite;

/*
 * define the structure of a list of satellites
 *
 */
typedef struct satellite_list {
  Satellite s;
  struct satellite_list *next;
} *Satellite_list;

/*
 * create array of 'marker' static satellites for equatorial ring
 */
#define EQUATORIAL_MARKERS 1080
Satellite marker[EQUATORIAL_MARKERS];

/*
 * define the constellation struct
 *
 */
typedef struct constellation {
  unsigned int n_sats, n_view_sats, n_tag_sats;
  Satellite_list satellites;
  CentralBody *pcb;
} Constellation;

/* declare routines */

Satellite satellite_create(int id, OrbitalElements *poe, CentralBody *pcb);
int satellite_name(Satellite s, char* name);

void satellite_oe_edited(Satellite s, CentralBody *pcb);
void satellite_destroy(Satellite s);
void satellite_compute_position(Satellite s, const double t,
				CentralBody *pcb);
void satellite_set_tag(Satellite, int);
int satellite_get_tag(Satellite);

int constellation_new_id(const Constellation *pconstellation);
int constellation_add(Constellation *pconstellation, Satellite s);
Satellite constellation_search(const Constellation *pconstellation, int);
void constellation_compute_positions(Constellation *pconstellation, double t);
void constellation_destroy_element(Constellation *pcosntellation, int);

#endif
/* !_SATELLITE_H_ */
