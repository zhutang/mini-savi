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
 * fisheye.h
 *
 * $Id: fisheye.h,v 1.21 2017/01/02 06:11:32 lloydwood Exp $
 */

#ifndef _FISHEYE_H_
#define _FISHEYE_H_

#include "Satellite.h"

void fisheye_location(LatLon *);

char * fisheye_on_cmd(int argc, char *argv[]);
char * fisheye_off_cmd(int argc, char *argv[]);
char * fisheye_labels_on_cmd(int argc, char *argv[]);
char * fisheye_labels_off_cmd(int argc, char *argv[]);
char * fisheye_reverse_view_on_cmd(int argc, char *argv[]);
char * fisheye_reverse_view_off_cmd(int argc, char *argv[]);
char * fisheye_print_inview_on_cmd(int argc, char *argv[]);
char * fisheye_print_inview_off_cmd(int argc, char *argv[]);
char * fisheye_names_on_cmd(int argc, char *argv[]);
char * fisheye_names_off_cmd(int argc, char *argv[]);
char * fisheye_numbers_on_cmd(int argc, char *argv[]);
char * fisheye_numbers_off_cmd(int argc, char *argv[]);
char * fisheye_times_on_cmd(int argc, char *argv[]);
char * fisheye_times_off_cmd(int argc, char *argv[]);
char * fisheye_viewpoint_on_cmd(int argc, char *argv[]);
char * fisheye_viewpoint_off_cmd(int argc, char *argv[]);
char * fisheye_set_cmd(int argc, char *argv[]);

void fisheye_display(const Satellite_list SL, unsigned int force,
			const CentralBody *pcb);

#endif
/* !_FISHEYE_H_ */
