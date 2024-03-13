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
 * constants.h
 *
 * $Id: constants.h,v 1.27 2017/01/02 06:11:32 lloydwood Exp $
 */

#ifndef NULL
#define NULL (void *)0
#endif

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

/*
 * SaVi's Tcl code uses 0 and 1 directly, rather than pulling globals
 * $TRUE and $FALSE into each procedure.
 */
#define TRUE 1
#define FALSE 0

/* for file-handling routines */
#define NOT_OK -1

#define HANDLE_FILE_SAVI "oogl/savi.oogl"
#define HANDLE_FILE_LOGO "oogl/savilogo.oogl"

#define AMBIENT_LIGHTING "oogl/ambient_light.oogl"
#define EARTH_YEAR (365.25*24*3600)
#define EARTH_TILT 23.5

#define TCL_DIR "tcl"

#ifndef PI
#define PI 3.14159265358979324
#define TWOPI 6.2831853071795865
#define HALFPI 1.570796326794897
#else
#define TWOPI PI*2
#define HALFPI PI/2
#endif

#define DEG_TO_RAD (PI/180.0)
#define RAD_TO_DEG (180.0/PI)
#define SECS_PER_DAY (3600*24)
#define ANOMALY_COMPUTATION_TOLERANCE 1e-5

/*  The gravitational parameter MU. Units are km^3/s^2  */
#define EARTH_MU 398601.2
#define EARTH_RADIUS 6378.14 /* km */

/*
 * Earth's rotation rate (in geocentric equatorial coordinates) is
 * (2*PI+2*PI/365.25) radians per day.
 * The second term accounts for the fact that the Earth is orbiting
 * the sun. The factor is 1/365.25 rather than 1/365 because of leap
 * year effects. The rate below is radians per second.
 */
#define EARTH_ROTATION_RATE (TWOPI*1.160576e-5)
#define EARTH_OBLATENESS 1.0826e-3

/*  Parameters for coverage display */
#define IMAGE_HEIGHT 300
#define IMAGE_WIDTH 600
#define IMAGE_LARGE_HEIGHT 512
#define IMAGE_LARGE_WIDTH  1024
#define IMAGE_MIN_HEIGHT 8
#define IMAGE_MAX_HEIGHT 2048

#define COVERAGE_ANGLE 0.0

/* set sensible diameter for fisheye view */
#define FISHEYE_DIAMETER 300

/* parameters for Geomview display */

/* number of segments in cone surface/footprint cannot be changed */
#define GV_FOOTPRINT_EDGE_SEGMENTS 16

/* increased for fidelity on highly elliptical orbits. */
#define GV_ORBIT_SEGMENTS 90

#endif
/* !_CONSTANTS_H_ */
