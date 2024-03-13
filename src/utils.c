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
 * utils.c
 *
 * $Id: utils.c,v 1.21 2017/01/02 06:11:32 lloydwood Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
/* for select */
#include <unistd.h>
#include <sys/time.h>

#include "constants.h"
#include "globals.h"
#include "utils.h"

/*
 * copyfile(stdout, "data.txt")
 *
 * Will copy the file data.txt to stdout.
 * Returns TRUE if successful, else FALSE.
 */
unsigned int
copyfile(FILE * out_fp, const char filename[])
{
  FILE *in_fp;
  char buf[LENGTH_STRING_BUFFER];

  if (!out_fp)
    return FALSE;

  if (NULL == (in_fp = fopen(filename, "r")))
    return FALSE;
  while (fgets(buf, LENGTH_STRING_BUFFER, in_fp)) {
    fprintf(out_fp, "%s", buf);
  }
  fflush(out_fp);
  fclose(in_fp);


  return TRUE;
}

/*
 * forward over comments and any newlines and white space
 *
 */
void
forward_over_comments(FILE * fp)
{
  int c = getc(fp);

  while ((c == '\n') || (c == ' ') || (c == '\t') || (c == '#')) {
    if (c == '#') {
      /* skip to end of line */
      do {
	c = getc(fp);
      } while ((c != '\n') && (c != EOF));
    }
    c = getc(fp);
  }
  ungetc(c, fp);
}

/*
 * identity
 *
 * Fills in a 4x4 matrix to be the identity matrix
 */
void
identity(double m[4][4])
{
  unsigned int i, j;

  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++)
      m[i][j] = ((i == j) ? 1.0 : 0.0);
}


/*
 * Return the norm of a CartesianCoordinate vector
 */
double
norm(const CartesianCoordinates * px)
{
  return (sqrt(px->x * px->x + px->y * px->y + px->z * px->z));
}

/*
 * Normalizes a CartesianCoordinate vector to unit length
 */
void
normalize(CartesianCoordinates * px)
{
  double l = norm(px);

  if (l == 0.0)
    return;
  px->x /= l;
  px->y /= l;
  px->z /= l;
}

/*
 * Return the dot-product of two CartesianCoordinate vectors.
 */
double
dot(const CartesianCoordinates * px, const CartesianCoordinates * py)
{
  return (px->x * py->x + px->y * py->y + px->z * py->z);
}


/*
 * The vector cross-product of two CartesianCoordinate vectors
 */
void
cross_product(CartesianCoordinates * pucv, const CartesianCoordinates * pu,
	      const CartesianCoordinates * pv)
{
  double pu_x = pu->x;
  double pu_y = pu->y;
  double pu_z = pu->z;

  double pv_x = pv->x;
  double pv_y = pv->y;
  double pv_z = pv->z;

  pucv->x = (pu_y * pv_z - pu_z * pv_y);
  pucv->y = (pu_z * pv_x - pu_x * pv_z);
  pucv->z = (pu_x * pv_y - pu_y * pv_x);
}

void
print_vec(const char msg[], const CartesianCoordinates * pv)
{
  fprintf(stderr, "%s = (%f,%f,%f)\n", msg, pv->x, pv->y, pv->z);
}

/*
 * Rotate a vector by angle theta about the z-axis.
 *
 * The rotation matrix is
 *
 *   cos(theta)    -sin(theta)    0
 *   sin(theta)     cos(theta)    0
 *       0              0         1
 *
 */
void
rotate_z(const CartesianCoordinates * v_old, const double theta,
	 CartesianCoordinates * v_new)
{
  double cos_theta = cos(theta);
  double sin_theta = sin(theta);
  double old_x = v_old->x;
  double old_y = v_old->y;

  v_new->x = cos_theta * old_x - sin_theta * old_y;
  v_new->y = sin_theta * old_x + cos_theta * old_y;
  v_new->z = v_old->z;
}



/*
 * error_format
 *
 * prints a string using formatting to stderr
 *   - a newline is automatically appended
 */
void
error_format(const char format[], const char msg[])
{
  fprintf(stderr, "\nSaVi: ");
  fprintf(stderr, format, msg);
}


/*
 * error_and_exit
 *
 * prints an error msg and exits program with value 1
 */
void
error_and_exit(const char msg[])
{
  error_format("%s", msg);
  error_format("%s", "Error! Exiting.");
  exit(1);
}

/*
 * error
 *
 * prints a string as an error message
 */
void
error(const char msg[])
{
  error_format("%s", msg);
}


void
millisleep(const unsigned int ms)
{

  struct timeval tv;

  tv.tv_usec = (ms % 1000) * 1000;
  tv.tv_sec = ms / 1000;
  select(0, NULL, NULL, NULL, &tv);
}
