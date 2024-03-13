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
 * orbit_utils.h
 *
 * $Id: orbit_utils.h,v 1.17 2017/01/02 06:11:32 lloydwood Exp $
 */

#ifndef _ORBIT_UTILS_H_
#define _ORBIT_UTILS_H_

#include "utils.h"

/*
 * Define a struct for orbital elements
 *
 * Note: we use time of periapsis whereas it is also common to use
 *       mean anomaly, MA.  To convert one to the other use the formula:
 *               T = -MA/360 * 2Pi * a^(3/2) / sqrt(mu)
 *       Where 2Pi * a^(3/2) / sqrt(mu) is of course the period of the orbit.
 */
typedef struct OrbitalElements {
  double
    a,              /* length of semi-major axis   */
    e,              /* eccentricity                */
    i,              /* inclination                 */
    Omega,          /* longitude of ascending node */
    omega,          /* argument of periapsis       */
    T;              /* time of periapsis           */
} OrbitalElements;


/*
 * Define a struct to hold information about a central body
 *
 */
typedef struct CentralBody {
  double
    radius,
    rotation_rate,
    mu,
    J2,
    orbital_period,
    tilt;
} CentralBody;

void oe_time_to_geocentric(CartesianCoordinates *px, const double t,
			     const OrbitalElements *poe, const CentralBody *pcb);
void oe_time_to_geocentric_spherical(SphericalCoordinates *px, const double t,
			     const OrbitalElements *poe, const CentralBody *pcb);
void spherical_to_transform(double T[4][4], const SphericalCoordinates *px,
			    const double scale);

void spherical_to_cartesian(CartesianCoordinates *px,
			    const SphericalCoordinates *py);
void cartesian_to_spherical(SphericalCoordinates *px,
			    const CartesianCoordinates *py);

void oe_time_J0_to_oe(OrbitalElements *final, const OrbitalElements *initial,
		      const double t, const CentralBody *pcb);
void oe_time_J2_to_oe(OrbitalElements *final, const OrbitalElements *initial,
		      const double t, const CentralBody *pcb);
void oe_time_to_oe(OrbitalElements *final, const OrbitalElements *initial,
		   const double t, const CentralBody *pcb);
void oe_time_J0_to_geocentric(CartesianCoordinates * px, const double t,
			      const OrbitalElements * poe, const CentralBody * pcb);
void oe_to_geocentric(CartesianCoordinates *px, const OrbitalElements *poe,
		      const CentralBody *pcb);

double oe_to_period(const OrbitalElements *poe, const CentralBody *pcb);

void perifocal_to_geocentric(CartesianCoordinates *, const CartesianCoordinates *,
			     const double, const double, const double);

void lat_lon_to_spherical (const double lat, const double lon, const double t,
			   const CentralBody *pcb, SphericalCoordinates *px);
void lat_lon_to_cartesian (const double lat, const double lon, const double t,
			   const CentralBody *pcb, CartesianCoordinates *px);
void spherical_to_lat_lon(LatLon *pl, const SphericalCoordinates *px,
			  const double t, const CentralBody *pcb);

double eccentric_anomaly_to_true_anomaly (const double E, const double e);
double time_to_true_anomaly (const double t, const CentralBody *pcb,
			     const double a, const double e, const double T);

int sat_to_fisheye (const CartesianCoordinates *ps, const double lat,
		    const double lon, const CentralBody *pcb, const double t,
		    double sky[2]);

void oe_info(FILE *, const OrbitalElements *poe, const CentralBody *pcb);

double oe_to_nodal_precession(const OrbitalElements *poe, const CentralBody *pcb);
double oe_to_apsidal_rotation(const OrbitalElements *poe, const CentralBody *pcb);
double oe_to_apoapsis_altitude(const OrbitalElements *poe, const CentralBody *pcb);
double oe_to_periapsis_altitude(const OrbitalElements *poe, const CentralBody *pcb);

double mean_anomaly(const double t, const double mu, const double a, const double T);

#endif
/* !_ORBIT_UTILS_H_ */
