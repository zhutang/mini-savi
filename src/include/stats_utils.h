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
 * stats_utils.h
 *
 * $Id: stats_utils.h,v 1.23 2017/01/02 06:11:32 lloydwood Exp $
 */

#ifndef _STATS_UTILS_H_
#define _STATS_UTILS_H_

#include <Satellite.h>

#define MAX(A, B) ((A) < (B) ? (B) : (A))
#define MIN(A, B) ((A) < (B) ? (A) : (B))

/* The coverage grid structure. */
typedef struct grid {
    int *data;
    int *noaccess;
    int *covered;
    unsigned int height, width, count;
  } grid;

grid *create_grid(int h, int w);
void destroy_grid(grid *g);
void clear_intensity(grid *g);
void fill_interval(grid * g, int value);
void decay_interval(grid *g);
void fill_grid(const Satellite_list SL, int projection, int fp_angle_type,
	       double footprint_angle, const CentralBody *pcb, grid *g);

void intensity_edges(int left[2], int right[2],
		     SphericalCoordinates *point,
		     int projection, grid *g);

void project_latlon_sinusoidal(double proj[2], LatLon *point);
void project_latlon_cylindrical(double proj[2], LatLon *point);
void project_latlon_unprojected(double proj[2], LatLon *point);
void project_latlon_spherical(double proj[2], LatLon *point);

#endif
/* !_STATS_UTILS_H_ */
