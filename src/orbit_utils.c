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
 * orbit_utils.c
 *
 * $Id: orbit_utils.c,v 1.25 2017/01/02 06:11:32 lloydwood Exp $
 */
#include <stdio.h>
#include <math.h>

#include "constants.h"
#include "globals.h"
#include "orbit_utils.h"
#include "stats_utils.h"

/*
 * converts perifocal coordinates y into geocentric equatorial Cartesian coords x
 * using the orbital elements: i, Omega, omega.
 */
void
perifocal_to_geocentric(CartesianCoordinates * x, const CartesianCoordinates * y,
			const double i, const double Omega, const double omega)
{
  double ci, si, cO, sO, co, so, tx, ty, tz;

  tx = y->x;
  ty = y->y;
  tz = y->z;

  ci = cos(i);
  si = sin(i);
  cO = cos(Omega);
  sO = sin(Omega);
  co = cos(omega);
  so = sin(omega);

  x->x =
    (cO * co - sO * so * ci) * tx + (-cO * so - sO * co * ci) * ty +
    sO * si * tz;
  x->y =
    (sO * co + cO * so * ci) * tx + (-sO * so + cO * co * ci) * ty -
    cO * si * tz;
  x->z = so * si * tx + co * si * ty + ci * tz;
}

/*
 * oe_time_J0_to_geocentric
 *
 * Converts orbital elements at time 0 to Cartesian coordinates
 * in the geocentric equatorial system at time t.
 *
 * This assumes the central body is a perfect sphere.
 * previously commented. Unused.
 */

void
oe_time_J0_to_geocentric(CartesianCoordinates * px,
			 const double t, const OrbitalElements * poe,
			 const CentralBody * pcb)
{
  CartesianCoordinates y;
  double nu, cosnu, r, a, e;

  a = poe->a;
  e = poe->e;

  nu = time_to_true_anomaly(t, pcb, a, e, poe->T);
  cosnu = cos(nu);
  r = a * (1 - e * e) / (1 + e * cosnu);
  y.x = r * cosnu;
  y.y = r * sin(nu);
  y.z = 0.0;

  perifocal_to_geocentric(px, &y, poe->i, poe->Omega, poe->omega);
}


/*
 * oe_to_geocentric
 *
 * Returns the position of a satellite from a set
 * of classical orbital elements
 *
 */
void
oe_to_geocentric(CartesianCoordinates * px, const OrbitalElements * poe,
		 const CentralBody * pcb)
{
  CartesianCoordinates y;
  double nu, cosnu, r, a, e;

  a = poe->a;
  e = poe->e;

  nu = time_to_true_anomaly(0.0, pcb, a, e, poe->T);
  cosnu = cos(nu);
  r = a * (1 - e * e) / (1 + e * cosnu);
  y.x = r * cosnu;
  y.y = r * sin(nu);
  y.z = 0.0;

  perifocal_to_geocentric(px, &y, poe->i, poe->Omega, poe->omega);
}

/*
 * oe_time_J0_to_oe
 *
 * Given orbital elements for the satellite at time 0,
 * returns orbital elements for the satellite at time t.
 *
 * This uses the Keplerian orbits formulation (spherical central body).
 *
 * (Since we use the orbital element T (time of periapsis passage)
 * flowing forward in time results in decreasing T.)
 */
void
oe_time_J0_to_oe(OrbitalElements * final, const OrbitalElements * initial,
		 const double t, const CentralBody * pcb)
{
  double d_T = -1.0;

  /* copy initial orbital elements to final orbital elements */
  *final = *initial;

  /* adjust the time until periapsis */
  final->T += t * d_T;
}

/*
 * oe_time_J2_to_oe
 *
 * Given orbital elements for the satellite at time 0,
 * returns orbital elements for the satellite at time t.
 *
 * This uses the J2 oblateness term in the model of the central body.
 *
 * See Szebehely, Adventures in Celestial Mechanics, Chap. 11
 * and U of Texas, Austin, Applied Orbital Mechanics course notes (Lundberg),
 * 3-22.
 *
 */
void
oe_time_J2_to_oe(OrbitalElements * final, const OrbitalElements * initial,
		 const double t, const CentralBody * pcb)
{
  double a = final->a = initial->a;
  double e = final->e = initial->e;
  double i = final->i = initial->i;

  double R_a_sqr = pcb->radius * pcb->radius / a / a;
  double one_minus_e_sqr = 1 - e * e;
  double factor = 0.75 * pcb->J2 * R_a_sqr
    / one_minus_e_sqr / one_minus_e_sqr;

  double n = sqrt(pcb->mu / a / a / a);
  double cosi = cos(i);
  double cosi2 = cosi * cosi;
  double d_Omega = -2.0 * factor * n * cosi;
  double d_omega = factor * n * (5.0 * cosi2 - 1.0);
  double d_T =
    -(1.0 + factor * sqrt(one_minus_e_sqr) * (3.0 * cosi2 - 1.0));

  /* adjust the longitude of ascending node */
  final->Omega = initial->Omega + t * d_Omega;
  /* adjust the argument of periapsis */
  final->omega = initial->omega + t * d_omega;
  /* adjust the time until periapsis */
  final->T = initial->T + t * d_T;
}

/*
 * oe_time_to_oe
 *
 * Given orbital elements for the satellite at time 0,
 * returns orbital elements for the satellite at time t.
 *
 */
void
oe_time_to_oe(OrbitalElements * final, const OrbitalElements * initial,
	      const double t, const CentralBody * pcb)
{

  switch (orbit_model) {
    case J0:
      oe_time_J0_to_oe(final, initial, t, pcb);
      break;
    default:
      /* J2 model is the current default if another model is not specified */
      oe_time_J2_to_oe(final, initial, t, pcb);
      break;
  }
}

/*
 * oe_time_to_geocentric
 *
 * Converts orbital elements at time 0 to spherical coordinates
 * in the geocentric equatorial system at time t.
 *
 * Uses the model that oe_time_to_oe uses.
 */
void
oe_time_to_geocentric(CartesianCoordinates * px, const double t,
		      const OrbitalElements * poe, const CentralBody * pcb)
{
  OrbitalElements oe;

  oe_time_to_oe(&oe, poe, t, pcb);
  oe_to_geocentric(px, &oe, pcb);
}

/*
 * oe_time_to_geocentric_spherical
 *
 * Converts orbital elements at time 0 to spherical coordinates
 * in the geocentric equatorial system at time t.
 *
 * Uses the whatever model oe_time_to_geocentric uses.
 */
void
oe_time_to_geocentric_spherical(SphericalCoordinates * px, const double t,
				const OrbitalElements * poe,
				const CentralBody * pcb)
{
  CartesianCoordinates y;

  oe_time_to_geocentric(&y, t, poe, pcb);
  cartesian_to_spherical(px, &y);
}

/*
 * spherical_to_transform
 *
 * creates a transformation matrix to move a satellite centered at the
 * origin to the specified point in spherical coordinates.  The vector
 * (0,0,-1) is mapped so that it points to the origin.  There is still
 * one degree of freedom in this specification!
 *
 */
void
spherical_to_transform(double m[4][4], const SphericalCoordinates * px,
		       const double scale)
{
  double theta, phi, ct, st, cp, sp, spct, spst, r0;

  const double zero = 0.0;

  theta = px->theta;
  ct = cos(theta);
  st = sin(theta);

  phi = px->phi;
  cp = cos(phi);
  sp = sin(phi);

  m[0][0] = cp * ct;
  m[0][1] = cp * st;
  m[0][2] = -sp;
  m[0][3] = zero;

  m[1][0] = -st;
  m[1][1] = ct;
  m[1][2] = zero;
  m[1][3] = zero;

  spct = sp * ct;
  spst = sp * st;

  m[2][0] = spct;
  m[2][1] = spst;
  m[2][2] = cp;
  m[2][3] = zero;

  r0 = px->r / scale;

  m[3][0] = r0 * spct;
  m[3][1] = r0 * spst;
  m[3][2] = r0 * cp;
  m[3][3] = 1.0;
}

/*
 * spherical_to_cartesian
 *
 * convert from spherical (r, theta, phi) to cartesian (x,y,z) coordinates.
 */
void
spherical_to_cartesian(CartesianCoordinates * u, const SphericalCoordinates * v)
{
  double r, theta, phi, sinphi;

  r = v->r;
  theta = v->theta;
  phi = v->phi;
  sinphi = sin(phi);

  u->x = r * cos(theta) * sinphi;
  u->y = r * sin(theta) * sinphi;
  u->z = r * cos(phi);
}

/*
 * converts geocentric cartesian coordinates to spherical coordinates
 * x = (r,theta,phi), where theta in (-Pi, Pi], phi in [0, Pi].
 */
void
cartesian_to_spherical(SphericalCoordinates * px, const CartesianCoordinates * py)
{
  double rsqr, x, y, z, x_r, x2_y2;

  x = py->x;
  y = py->y;
  x2_y2 = x * x + y * y;

  if (y < 0.0) {
    px->theta = -acos(x / sqrt(x2_y2));
  } else {
    px->theta = acos(x / sqrt(x2_y2));
  }

  z = py->z;
  rsqr = x2_y2 + z * z;
  px->r = x_r = sqrt(rsqr);
  px->phi = acos(z / x_r);

}

/*
 * lat_lon_to_spherical
 *
 * Convert geocentric latitude (NOTE: NOT geodetic latitude. Will need to
 * account for oblateness of earth later.) and longitude coordinates to
 * NORMALIZED (length=1) geocentric equatorial spherical coordinates
 * (r, theta, phi).
 *
 * We assume that longitude equals geocentric equatorial theta at t=0.
 */
void
lat_lon_to_spherical(const double lat, const double lon, const double t,
		     const CentralBody * pcb, SphericalCoordinates * px)
{
  px->r = 1.0;			/* Geocentric equatorial spherical */
  px->theta = lon * DEG_TO_RAD + pcb->rotation_rate * t;	/* coordinates. */
  px->phi = (90.0 - lat) * DEG_TO_RAD;
}

/*
 * lat_lon_to_cartesian
 *
 * Convert geocentric latitude (NOTE: NOT geodetic latitude. Will need to
 * account for oblateness of earth later.) and longitude coordinates to
 * NORMALIZED (length=1) geocentric equatorial cartesian coordinates.
 *
 * We assume that longitude equals geocentric equatorial theta at t=0.
 */
void
lat_lon_to_cartesian(const double lat, const double lon, const double t,
		     const CentralBody * pcb, CartesianCoordinates * px)
{
  SphericalCoordinates y;

  lat_lon_to_spherical(lat, lon, t, pcb, &y);
  spherical_to_cartesian(px, &y);
}

/*
 * spherical_to_lat_lon
 *
 * Converts spherical (geocentric equatorial) coordinates to
 * the latitude and longitude of the point on earth it is over
 */
void
spherical_to_lat_lon(LatLon * pl, const SphericalCoordinates * px,
		     const double t, const CentralBody * pcb)
{
  double x;
  x = (px->theta - pcb->rotation_rate * t) * RAD_TO_DEG - 180.0;
  if (x > 0)
    pl->lon = fmod(x, 360.0) - 180.0;
  else
    pl->lon = fmod(x, 360.0) + 180.0;
  pl->lat = 90.0 - px->phi * RAD_TO_DEG;
}

/*
 * Given a time, return true anomaly.
 */
double
time_to_true_anomaly(const double t, const CentralBody * pcb,
		     const double a, const double e, const double T)
{
  double M, M_n, E, incr;
/*
 * E is the eccentric anomaly, M = E-e*sinE is the mean anomaly.  The
 * time of flight equation, t-T = Msqrt(a^3/mu), gives M in terms of
 * time, and we solve for E in terms of M by Newton's method (the Kepler
 * problem).  The calculation is trivial for circular orbits, and is handled
 * separately.
 */

  if (e == 0.0) {
    /* mean anomaly = eccentric anomaly = true anomaly */
    return mean_anomaly(t, pcb->mu, a, T);
  }

  if (e < 0.1) {
    /* mean anomaly is a good seed for Newton's method */
    M = mean_anomaly(t, pcb->mu, a, T);
    E = M;
    M_n = M - e * sin(M);
    incr = (M - M_n) / (1 - e * cos(E));
    while (fabs(incr) > ANOMALY_COMPUTATION_TOLERANCE) {
      E += incr;
      M_n = E - e * sin(E);
      incr = (M - M_n) / (1 - e * cos(E));
    }
  } else {
    /* will force each iterate of Newton's method to remain in 0 to 2pi */
    M = mean_anomaly(t, pcb->mu, a, T);
    E = M;
    M_n = M - e * sin(M);
    incr = (M - M_n) / (1 - e * cos(E));
    while (fabs(incr) > ANOMALY_COMPUTATION_TOLERANCE) {
      E += incr;
      if (E < 0) {
	E = (E - incr) / 2.0;
      } else if (E > TWOPI) {
	E = (E - incr + TWOPI) / 2.0;
      }
      M_n = E - e * sin(E);
      incr = (M - M_n) / (1 - e * cos(E));
    }
  }

  return eccentric_anomaly_to_true_anomaly(E, e);
}

/*
 * Convert eccentric anomaly to true anomaly.
 * E is the eccentric anomaly, e is the eccentricity of the orbit.
 *
 * Note:  If e=0, then eccentric anomaly equals true anomaly and
 *   there is no reason to call this routine!
 */
double
eccentric_anomaly_to_true_anomaly(const double E, const double e)
{
  if (sin(E) >= 0.0)
    return acos((e - cos(E)) / (e * cos(E) - 1));
  else
    return 2 * PI - acos((e - cos(E)) / (e * cos(E) - 1));
}

/*
 * sat_to_fisheye
 *
 * Given a satellite position vector (cartesian geocentric-equatorial
 * coordinates) and time, compute the fisheye coordinates of that satellite as
 * viewed from a given latitude/longitude on the earth.
 *
 * If the spherical coordinates of the satellite as viewed from the
 * ground are (phi, theta), where phi is measured from vertical, and
 * theta is measured from east, then the fisheye coordinates are given
 * by 1/(PI/2)*(-phi*cos(theta), phi*sin(theta)).  Thus coordinates
 * lie in the unit disk, and are oriented so that north is up, south is
 * down, east is LEFT, west is RIGHT. (This is the correct orientation
 * when looking up.)
 *
 * Value of function is TRUE if satellite is in view, FALSE otherwise.
 */
int
sat_to_fisheye(const CartesianCoordinates * ps, const double lat, const double lon,
	       const CentralBody * pcb, const double t, double sky[2])
{
  CartesianCoordinates p, sp, e, n;
  double phi, alpha, radius;

  lat_lon_to_cartesian(lat, lon, t, pcb, &p);

  /* sp is the vector from point on earth to satellite.
     Scale satellite vector by earth radius since we assume |p|=1.  */

  radius = pcb->radius;
  sp.x = ps->x / radius - p.x;
  sp.y = ps->y / radius - p.y;
  sp.z = ps->z / radius - p.z;

  normalize(&sp);
  normalize(&p);

  phi = acos(dot(&p, &sp));	/* The view angle from vertical. */

  if (debug) fprintf(stderr, "Angle from vertical: %f\n", phi * RAD_TO_DEG);

  if (phi > PI / 2.0)
    return FALSE;

  /* Compute "east" and "north" vectors spanning the tangent plane at p.
     For the north pole, take east to be direction of 0 degrees long.
     For the south pole, take east to be direction of 180 degrees long.   */

  if (fabs(lat) == 90.0) {
    lat_lon_to_cartesian(0.0, lon + 90.0, t, pcb, &e);
  } else {
    e.x = -p.y;			/* The "east" vector is the projection of p onto the    */
    e.y = p.x;			/* equatorial plane, then rotated counterclockwise pi/2 */
    e.z = 0.0;			/* and normalized.                                      */
  }
  normalize(&e);
  cross_product(&n, &p, &e);	/* "North" is then p x e.  */

  /* project sp onto plane defined by n,e by subtracting out components of p */
  alpha = dot(&p, &sp);
  sp.x -= alpha * p.x;
  sp.y -= alpha * p.y;
  sp.z -= alpha * p.z;
  normalize(&sp);

  /* Cos(theta) equals the projection of the new sp onto e,
     or the dot product of sp and e. Sin(theta) equals the
     projection of sp onto n. */

  sky[0] = -2.0 / PI * phi * dot(&sp, &e);
  sky[1] = 2.0 / PI * phi * dot(&sp, &n);

  if (debug) {
    print_vec("sp", &sp);
    print_vec("p", &p);
    print_vec("e", &e);
    print_vec("n", &n);
    fprintf(stderr, "Norms: sp %f p %f e %f n %f\n", norm(&sp), norm(&p),
	    norm(&e), norm(&n));
    fprintf(stderr, "sky = (%f , %f )\n", sky[0], sky[1]);
  }

  return TRUE;
}

/*
 * oe_to_period
 *
 * Returns the period of a classical orbit given its orbital elements
 *
 */
double
oe_to_period(const OrbitalElements * poe, const CentralBody * pcb)
{
  return (TWOPI * pow(poe->a, 1.5) / sqrt(pcb->mu));
}

/*
 * oe_info
 *
 * Writes to file information about the orbital elements
 *
 */
void
oe_info(FILE * out, const OrbitalElements * poe, const CentralBody * pcb)
{
  double period = oe_to_period(poe, pcb);

  /* output orbital elements */
  fprintf(out, "a=%f e=%f i=%f Om=%f om=%f T=%f ",
	  poe->a, poe->e, poe->i, poe->Omega, poe->omega, poe->T);

  /* output period of orbit in hours */
  fprintf(out, " (%f=%f hrs)\n", period, period / 3600.0);
}

/*
 * oe_to_nodal_precession
 *
 * Returns the nodal precession for the J2 model
 *
 */
double
oe_to_nodal_precession(const OrbitalElements * poe, const CentralBody * pcb)
{
  double a = poe->a;

  double R_a_sqr = pcb->radius * pcb->radius / a / a;
  double one_minus_e_sqr = 1 - poe->e * poe->e;
  double factor =
    0.75 * pcb->J2 * R_a_sqr / one_minus_e_sqr / one_minus_e_sqr;
  double n = sqrt(pcb->mu / a / a / a);
  double cosi = cos(poe->i);

  double nodal_precession = -2.0 * factor * n * cosi;

  if (debug) fprintf(stderr, "nodalprecession = %f\n", nodal_precession);

  return (nodal_precession);
}

/*
 * oe_to_apsidal_rotation
 *
 * Returns the apsidal rotation for the J2 model
 *
 */
double
oe_to_apsidal_rotation(const OrbitalElements * poe, const CentralBody * pcb)
{
  double a = poe->a;

  double R_a_sqr = pcb->radius * pcb->radius / a / a;
  double one_minus_e_sqr = 1 - poe->e * poe->e;
  double factor =
    0.75 * pcb->J2 * R_a_sqr / one_minus_e_sqr / one_minus_e_sqr;
  double n = sqrt(pcb->mu / a / a / a);
  double cosi = cos(poe->i);

  double apsidal_rotation = factor * n * (5.0 * cosi * cosi - 1.0);

  if (debug) fprintf(stderr, "apsidal rotation = %f\n", apsidal_rotation);

  return (apsidal_rotation);
}

/*
 * oe_to_apoapsis_altitude
 *
 * Returns the apoapsis altitude, the height from the surface of the central
 * body to the satellite at its furthest point in the orbit.
 *
 */
double
oe_to_apoapsis_altitude(const OrbitalElements * poe, const CentralBody * pcb)
{
  return (poe->a * (1 + poe->e) - pcb->radius);
}

/*
 * oe_to_periapsis_altitude
 *
 * Returns the periapsis altitude, the height from the surface of the central
 * body to the satellite during closest approach.
 *
 */
double
oe_to_periapsis_altitude(const OrbitalElements * poe, const CentralBody * pcb)
{
  return (poe->a * (1 - poe->e) - pcb->radius);
}

/*
 * mean_anomaly
 *
 * Computes the mean anomaly in terms of current time t, gravitational
 * parameter mu, and the orbital elements a and T, and returns it in the
 * range 0 to 2*pi.
 *
 * M = sqrt(mu/(a*a*a))*(t-T) mod 2*pi
 *
 */
double
mean_anomaly(const double t, const double mu, const double a, const double T)
{
  double M = fmod(sqrt(mu / (a * a * a)) * (t - T), TWOPI);

  /* if negative add two pi to put in correct range */
  if (M < 0.0)
    M += TWOPI;

  return M;
}
