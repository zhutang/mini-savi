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
 * stats_utils.c
 *
 * $Id: stats_utils.c,v 1.46 2017/06/07 21:34:22 lloydwood Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "sats.h"
#include "constants.h"
#include "globals.h"
#include "stats_utils.h"
#include "orbit_utils.h"
#include "utils.h"


static void orient_circle(double t, CartesianCoordinates *pc,
			  CartesianCoordinates *pu,
			  CartesianCoordinates *pv,
			  const CentralBody *pcb);
static void footprint_circle(CartesianCoordinates *pc,
			     CartesianCoordinates *pu,
			     CartesianCoordinates *pv,
			     int fp_angle_type, double footprint_angle,
			     CartesianCoordinates *py, const CentralBody *pcb);
static double footprint_circle_radius(double l,
				      int fp_angle_type,
				      double footprint_angle);
static void intensity_circle_footprint(CartesianCoordinates *pc,
				       CartesianCoordinates *pu,
				       CartesianCoordinates *pv,
				       int current_projection, grid *g,
				       int special);

static void increment_intensity (int row, int column, grid *g, int special);
static void circle_point(SphericalCoordinates *point,
			 CartesianCoordinates *pc,
			 CartesianCoordinates *pu,
			 CartesianCoordinates *pv,
			 double t);
static void intensity_index(int grid_index[2], SphericalCoordinates *point,
			    int current_projection, grid *g);
static void cap_pole(CartesianCoordinates *pc, CartesianCoordinates *pu,
		     CartesianCoordinates *pv,
		     int current_projection, grid *g, int special);
static void project_sinusoidal(double proj[2], SphericalCoordinates *point);
static void project_cylindrical(double proj[2], SphericalCoordinates *point);
static void project_unprojected(double proj[2], SphericalCoordinates *point);
static void project_spherical(double proj[2], SphericalCoordinates *point);

/*
 * Initialize the coverage grid
 */
grid*
create_grid(int h, int w)
{
  grid *pg;

  pg=(grid *) malloc(sizeof(grid));

  pg->data=(int *) malloc(h * w *sizeof(int));
  pg->noaccess=(int *) malloc(h * w * sizeof(int));
  pg->covered=(int *) malloc(h * sizeof(int));

  pg->height=h;
  pg->width=w;
  pg->count = 0;

  return pg;
}

/*
 * Free space allocated to a coverage grid.
 */
void
destroy_grid(grid *g)
{
  free(g->data);
  free(g->noaccess);
  free(g->covered);

  free(g);
}
/*
 * Clear the intensity array.
 */
void
clear_intensity(grid * g)
{
  unsigned int block;

  block = sizeof(int) * g->height * g->width;
  memset(g->data, 0, (size_t) block);
  memset(g->covered, 0, (size_t) sizeof(int) * g->height);
  g->count = 0;
}

/*
 * fill interval-decay with fixed value meaning many intervals since coverage
 */
void
fill_interval(grid * g, int value)
{
  unsigned int block;

  block = sizeof(int) * g->height * g->width;
  memset(g->noaccess, value, (size_t) block);
}

/*
 * Increment everything on the interval grid towards decay.
 */
void
decay_interval(grid * g)
{
  unsigned int i;
  unsigned int image_size = g->height * g->width;

  for (i = 0; i < image_size; i++) {
  	(g->noaccess[i])++;
  }
}

/*
 * Fill out the coverage grid.
 */
void
fill_grid(const Satellite_list SL, int current_projection, int fp_angle_type,
	   double footprint_angle, const CentralBody *pcb, grid *g)
{
  CartesianCoordinates c, u, v;
  Satellite_list sl;
  double angle, a;

  /* first satellite is special sunlight approximation */
  int sunlight = TRUE;
  if (fp_angle_type == MASK_ELEVATION) {
    angle = 0;
  } else {
    angle = HALFPI;
  }

  for (sl = SL; NULL != sl; sl = sl->next) {
    if (sl->s->can_display_coverage) {
      a = sl->s->x_S.r - pcb->radius;
      if (!sunlight && ((min_transmit_altitude > 0)||(max_transmit_altitude > 0))) {
        if (a <= min_transmit_altitude) {
           continue;
        }
        if (a >= max_transmit_altitude) {
           continue;
        }
      }
      footprint_circle(&c, &u, &v, fp_angle_type, angle,
		       &(sl->s->x_C), pcb);
      orient_circle(sl->s->t, &c, &u, &v, pcb);
      intensity_circle_footprint(&c, &u, &v, current_projection, g, sunlight);
    }
    if (sunlight) {
      angle = footprint_angle;
      sunlight = FALSE;
    }
  }
}

/*
 * Given orbital elements, compute circular footprint of a satellite.
 *
 * The circle is normalized to lie on a unit radius reference sphere, and
 * is specified by a center c and basis vectors u and v, in normalized
 * geocentric coordinates.  The parametrization is thus c+cos(t)u + sin(t)v.
 */
static void
footprint_circle(CartesianCoordinates *pc, CartesianCoordinates *pu,
		 CartesianCoordinates *pv,
		 int fp_angle_type, double footprint_angle,
		 CartesianCoordinates *py, const CentralBody *pcb)
{
  SphericalCoordinates z;
  CartesianCoordinates w;
  double ly, l, d, r, ip;

/* The center first */

  ly = norm(py);
  l = ly/pcb->radius;
  r = footprint_circle_radius( l, fp_angle_type, footprint_angle);
  d = sqrt(1-r*r);
  ip = d / ly;
  pc->x = py->x*ip;
  pc->y = py->y*ip;
  pc->z = py->z*ip;

/* Now the "up" basis vector. */

  cartesian_to_spherical(&z, py);
  z.r = 1;
  z.phi -= asin(r);

  if (debug) fprintf(stderr, "r=%.4f dphi=%.4f y=(%.2f, %.2f, %.2f)\n", \
	  r, asin(r), z.r, z.theta, z.phi);

  spherical_to_cartesian(&w, &z);
  pu->x = w.x-pc->x;
  pu->y = w.y-pc->y;
  pu->z = w.z-pc->z;

/* The final basis vector we can take to be the cross-product c x u,
 * normalized to have same length as u (that is, r).  Notice, then, that
 * v points to the "left". */

  cross_product( pv, pc, pu );
  pv->x /= d;
  pv->y /= d;
  pv->z /= d;
}

/*
 * orient_circle
 *
 * For displaying onto the coverage map, the footprint circle vectors need
 * to be rotated around the z-axis so that Longitude_Center_Line appears
 * at geocentric equatorial theta=0.
 */
static void
orient_circle(double t, CartesianCoordinates *pc,
	      CartesianCoordinates *pu, CartesianCoordinates *pv,
	      const CentralBody *pcb)
{
  SphericalCoordinates p;
  CartesianCoordinates w;

/* Find the geocentric equatorial spherical coordinates of the specified */
/* longitude at the current time.  Then rotate all vectors by minus this */
/* angle about the z-axis.                                               */

  lat_lon_to_spherical (0.0, (double) Longitude_Center_Line, t, pcb, &p);

  rotate_z(pc, -p.theta, &w);
  pc->x = w.x;
  pc->y = w.y;
  pc->z = w.z;

  rotate_z(pu, -p.theta, &w);
  pu->x = w.x;
  pu->y = w.y;
  pu->z = w.z;

  rotate_z(pv, -p.theta, &w);
  pv->x = w.x;
  pv->y = w.y;
  pv->z = w.z;
}

/*
 * Given the normalized distance to the spacecraft, compute the radius of
 * a circular footprint.
 */
static double
footprint_circle_radius( double L, int fp_angle_type, double footprint_angle)
{
  double sa = sin(footprint_angle);
  double ca = cos(footprint_angle);

  if (fp_angle_type == MASK_ELEVATION) {

      if (L < 1) {
          /*
           * Avoid NaN oddities and overflows when satellite drops below
	   * Earth surface.
           * Only do this for mask elevation, because the invert effect for
           * satellite cones is very cool and safe.
           */
          L = 1;
      }

    if (debug) fprintf(stderr, "Mask elevation = %f radians.\n", footprint_angle);

    return( ca*(sqrt(1-ca*ca/L/L)-sa/L) );
  } else {

    if (debug) fprintf(stderr, "Sat cone angle = %f radians.\n", footprint_angle);

    if ( L >= 1/sa )
      return( sqrt(L*L-1)/L );
    else
      return( sa*(L*ca-sqrt(1-L*L*sa*sa)) );
  }
}

/*
 * Increment the intensity image, circular footprint.
 */
static void
intensity_circle_footprint(CartesianCoordinates *pc, CartesianCoordinates *pu,
			   CartesianCoordinates *pv, int current_projection,
			   grid *g, int special)
{
  double t, dt, t_final;
  SphericalCoordinates point;
  int j, row, prev_row, left[2], right[2], left_edge[2], right_edge[2];

  /* compute phi increment so that we fill in each row of grid */
  switch (current_projection) {
  case UNPROJECTED:
  case UNPROJECTED_MASK:
  case SINUSOIDAL:
  case SINUSOIDAL_90:
    dt = PI/(g->height-1)/norm(pu);
    break;
  case SPHERICAL:
  case SPHERICAL_90:
  case CYLINDRICAL:
  default:
    dt = 2.0/(g->height-1)/norm(pu);
  }

  t_final = PI;

  circle_point(&point, pc, pu, pv, 0.0);
  intensity_index(left, &point, current_projection, g);
  increment_intensity(left[1], left[0], g, special);
  row=left[1];
  prev_row=row;

  for (t=dt; t<t_final; t+=dt) {
    circle_point(&point, pc, pu, pv, t);
    intensity_index(left, &point, current_projection, g);
    row = left[1];
    if (row != prev_row) {
      if ( abs(row-prev_row) > 1 ) {
	fprintf(stderr, "Intensity: row skipped. Prev=%i,current=%i.\n",
		prev_row, row);
      }
      circle_point(&point, pc, pu, pv, TWOPI-t);
      intensity_index(right, &point, current_projection, g);

      if (left[0] <= right[0]) {
	for (j = left[0]; j <= right[0]; j++) {
	  increment_intensity(row,j,g, special);
	}
      } else {
	intensity_edges(left_edge, right_edge, &point, current_projection, g);
	for(j = left_edge[0]; j <= right[0]; j++) {
	  increment_intensity(row,j,g, special);
	}
	for(j = left[0]; j < right_edge[0]; j++) {
	  increment_intensity(row,j,g, special);
	}
      }
      prev_row = row;
    }
  }

  /*
   * Footprint caps one of the poles if projection of center vector c onto
   * x,y plane has magnitude less than magnitude of projection of circle basis
   * vector u (u always points in direction of north pole from c).
   */
  if ( pc->x*pc->x+pc->y*pc->y < pu->x*pu->x+pu->y*pu->y ) {
    cap_pole( pc, pu, pv, current_projection, g, special);
  }
}

/*
 * Increment the intensity image at given row and column.
 * (row ranges from 0 to g->height-1,
 *  column from 0 to g->width-1.)
 */
static void
increment_intensity(int row, int column, grid *g, int special)
{
  unsigned int k;

  k = g->width * row + column;

  if (!special && ( 0 == (g->data[k])++) ) {
    if (sun_flag) {
      (g->noaccess[k]) = NUM_COLORS - 1;
    } else {
      (g->noaccess[k]) = 0;
    }
    (g->covered[row])++;
    (g->count)++;
  } else {
      if (sun_flag) {
	(g->noaccess[k]) = NUM_COLORS - 1;
      }
      if (!special) {
	(g->covered[row])++;
        (g->count)++;
      }
  }
}

/*
 * For time t, return the spherical coordinates of the point
 * c+cos(t)u+sin(t)v on circle with center c and basis vectors u and v.
 */
static void
circle_point(SphericalCoordinates *point, CartesianCoordinates *pc,
	     CartesianCoordinates *pu, CartesianCoordinates *pv, double t )
{
  CartesianCoordinates temp;
  float ct = cos(t), st = sin(t);

  temp.x = pc->x+ct*pu->x+st*pv->x;
  temp.y = pc->y+ct*pu->y+st*pv->y;
  temp.z = pc->z+ct*pu->z+st*pv->z;
  cartesian_to_spherical(point, &temp);
}

/*
 * Given point on unit sphere in spherical coordinates, return corresponding
 * index into coverage intensity grid.
 *        grid_index[0]=column index, grid_index[1]=row index.
 */
static void
intensity_index(int grid_index[2], SphericalCoordinates *point,
		int current_projection, grid *g)
{
  double proj[2];

  switch (current_projection) {
  case UNPROJECTED:
  case UNPROJECTED_MASK:
    project_unprojected(proj, point);
    break;
  case SINUSOIDAL:
  case SINUSOIDAL_90:
    project_sinusoidal(proj, point);
    break;
  case SPHERICAL:
  case SPHERICAL_90:
    project_spherical(proj,point);
    break;
  case CYLINDRICAL:
  default:
    project_cylindrical(proj, point);
  }

  /* It's assumed that proj[0] ranges from -PI to PI no matter what. */
  grid_index[0]=((int) ((proj[0]+PI)*(g->width)/TWOPI));
  grid_index[1]=((int) (proj[1]*(g->height-1)/PI));
}

/*
 * Increment intensity image for a polar cap.
 *
 * The routine increments those image pixels corresponding to the largest
 * disk centered at the pole contained within the footprint.  We're helped
 * here by the fact that u, when based at the tip of the center vector c,
 * always points in the direction of the north pole.  So for north pole
 * caps, we simply increment rows 0 through the row corresponding to c+u.
 *
 * For south poles, the vector c-u is the closest point in the footprint
 * to the pole, so we increment that row through the final row.
 */
static void
cap_pole(CartesianCoordinates *pc, CartesianCoordinates *pu,
	 CartesianCoordinates *pv, int current_projection, grid *g, int special)
{
  SphericalCoordinates point;
  int grid_index[2], left[2], right[2], bottom, i, j;
  const double dphi=PI/g->height;

  if (pc->z > 0.0) {  /* Cap covers north pole. */
    circle_point(&point, pc, pu, pv, 0.0);
    intensity_index(grid_index, &point, current_projection, g);

    if (debug) {
      fprintf(stderr, "North pole cap: begin %d, end %d\n", 0, grid_index[1]);
      fprintf(stderr, "Size of cap: theta=%f.2\n", point.theta);
    }

    for (i = 0; i < grid_index[1]+1; i++) {

      if (debug) fprintf(stderr, "row %d  ", i);

      intensity_edges(left, right, &point, current_projection, g);
      if (i == 0 && right[0] <= left[0]) right[0] = left[0] + 1;

      if (debug) fprintf(stderr, "columns %d to %d\n", left[0], right[0]);

      for (j = left[0]; j < right[0]; j++) {
	increment_intensity(i,j,g, special);
      }
      point.phi += dphi;
    }
  } else { /* South pole. But u still points towards north pole. */
    circle_point(&point, pc, pu, pv, PI); /* t=PI gets point on other side */
    intensity_index(grid_index, &point, current_projection, g); /* of south pole. */
    bottom = g->height;

    if (debug) fprintf(stderr, "South pole cap: begin %d, end %d\n",
		       grid_index[1]+1, bottom);

    for (i = grid_index[1]+1; i< bottom; i++) {

      if (debug) fprintf(stderr, "row %d  ", i);

      intensity_edges(left, right, &point, current_projection, g);
      if (i == bottom - 1 && right[0] <= left[0]) right[0] = left[0] + 1;

      if (debug) fprintf(stderr, "columns %d to %d\n", left[0], right[0]);

      for(j = left[0]; j < right[0]; j++) {
	increment_intensity(i,j,g, special);
      }
      point.phi += dphi;
    }
  }
}

/*
 * Given spherical coordinates on unit sphere, compute area-preserving
 * sinusoidal 2-d projection. Map is (1,theta,phi) -> (theta*sin(phi), phi).
 * proj[0] in [-Pi, Pi], proj[1] in [0, Pi].
 */
static void
project_sinusoidal(double proj[2], SphericalCoordinates *point)
{
  proj[0]=point->theta*sin(point->phi);
  proj[1]=point->phi;
}

/*
 * Given spherical coordinates on unit sphere, compute "area-preserving"
 * rectangular 2-d projection. Map is (1,theta,phi) -> (theta, Pi/2 * (1-cos(phi))).
 * proj[0] in [-Pi, Pi], proj[1] in [0, Pi] with phi=0 corresponding to
 * y=0.  (dA on sphere = 2/Pi dx dy)
 */
static void
project_cylindrical(double proj[2], SphericalCoordinates *point)
{
  proj[0]=point->theta;
  proj[1]=HALFPI*(1.0-cos(point->phi));
}

/*
 * Given spherical coordinates on unit sphere, compute unprojected
 * rectangular 2-d projection. Map is (1,theta,phi) -> (theta, phi).
 * proj[0] in [-Pi, Pi], proj[1] in [0, Pi].
 */
static void
project_unprojected(double proj[2], SphericalCoordinates *point)
{
  proj[0]=point->theta;
  proj[1]=point->phi;
}

/*
 * Given spherical coordinates on unit sphere, compute spherical
 * globes, two to a rectangle. Map is (1,theta,phi) -> (theta, phi).
 * proj[0] in [-Pi, Pi], proj[1] in [0, Pi].
 */
static void
project_spherical(double proj[2], SphericalCoordinates *point)
{
  if (point->theta < 0) {
    proj[0] = -HALFPI + HALFPI * sin(point->theta + HALFPI) * cos(HALFPI-point->phi);
  } else {
    proj[0] = HALFPI - HALFPI * sin(point->theta + HALFPI) * cos(HALFPI-point->phi);
  }
  proj[1]=HALFPI*(1.0-cos(point->phi));
}


/*
 * Given lat/lon coordinates on sphere, compute area-preserving
 * sinusoidal 2-d projection. Map is (lon, lat) -> (lon*sin(90-lat), 90-lat).
 * proj[0] in [-180, 180], proj[1] in [0,180].
 */
void
project_latlon_sinusoidal(double proj[2], LatLon *point)
{
  proj[0]=point->lon*(cos(DEG_TO_RAD*(point->lat)));
  proj[1]=90.0-point->lat;
}

/*
 * Given lat/lon coordinates on sphere, compute "area-preserving"
 * rectangular 2-d projection. Map is
 * (lon, lat) -> (lon, 90(1-cos(90-lat))).
 * proj[0] in [-180,180], proj[1] in [0,180] with lon=0 corresponding to
 * y=0.  (dA on sphere = 2/Pi dx dy)
 */
void
project_latlon_cylindrical(double proj[2], LatLon *point)
{
  proj[0]=point->lon;
  proj[1]=90.0*(1-sin(DEG_TO_RAD*(point->lat)));
}

/*
 * Given lat/lon coordinates on sphere, compute rectangular
 * unprojected 2-d projection. Map is (lon, lat) -> (lon*sin(90-lat), 90-lat).
 * proj[0] in [-180, 180], proj[1] in [0,180].
 */
void
project_latlon_unprojected(double proj[2], LatLon *point)
{
  proj[0]=point->lon;
  proj[1]=90.0-point->lat;
}

/*
 * Given lat/lon coordinates on sphere, compute spherical globes
 * on 2-d projection. Map is (lon, lat) -> broken into two hemispheres.
 * proj[0] in [-180, 180], proj[1] in [0,180].
 */
void
project_latlon_spherical(double proj[2], LatLon *point)
{
  if (point->lon < 0) {
    proj[0] =-90 + 90.0*sin(DEG_TO_RAD*(point->lon + 90)) * cos(DEG_TO_RAD*(point->lat));
  } else {
    proj[0] = 90 - 90.0*sin(DEG_TO_RAD*(point->lon + 90)) * cos(DEG_TO_RAD*(point->lat));
  }
  proj[1] = 90.0*(1-sin(DEG_TO_RAD*(point->lat)));
}

/*
 * Compute the left and right edge points in the projection map at the same
 * height as the projection of a point given in spherical coords (r, theta,
 * phi).
 *
 * left, right[0] = x (row) position
 * left, right[1] = y (column) position
 */
void
intensity_edges(int left[2], int right[2], SphericalCoordinates *point,
		int current_projection, grid *g)
{
  SphericalCoordinates temp;

  temp.r = point->r;
  temp.theta = PI;
  temp.phi = point->phi;
  intensity_index(right, &temp, current_projection, g);
  left[1]=right[1];
  left[0]=g->width-right[0];
}
