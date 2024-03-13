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
 * utils.h
 *
 */
#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>

/*
 * Define structs for Cartesian and Spherical coordinates
 */

typedef struct CartesianCoordinates {
  double x, y, z;
} CartesianCoordinates;

typedef struct SphericalCoordinates {
  double r, theta, phi;
} SphericalCoordinates;

typedef struct LatLon {
  double lat, lon;
} LatLon;

unsigned int copyfile(FILE *, const char []);
void forward_over_comments(FILE *);
void identity(double [4][4]);
void error_format(const char[], const char[]);
void error_and_exit(const char[]);
void error(const char[]);

double norm(const CartesianCoordinates *px);
void normalize(CartesianCoordinates *px);
double dot(const CartesianCoordinates *px, const CartesianCoordinates *py);
void cross_product(CartesianCoordinates *pucv, const CartesianCoordinates *pu,
		   const CartesianCoordinates *pv);
void print_vec(const char msg[], const CartesianCoordinates *pv);
void rotate_z(const CartesianCoordinates *v_old, const double theta,
	      CartesianCoordinates *v_new);

void millisleep(const unsigned int ms);

#endif
/* !_UTILS_H_ */
