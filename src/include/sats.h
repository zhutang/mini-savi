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
 * sats.h
 *
 * $Id: sats.h,v 1.22 2017/01/02 06:11:32 lloydwood Exp $
 */

#ifndef _SATS_H_
#define _SATS_H_

#include <Satellite.h>

char *forwards_cmd(int argc, char *argv[]);
char *backwards_cmd(int argc, char *argv[]);
char *forwards_step_cmd(int argc, char *argv[]);
char *backwards_step_cmd(int argc, char *argv[]);
char *reset_cmd(int argc, char *argv[]);
char *stop_cmd(int argc, char *argv[]);

void set_realtime(void);
char *realtime_on_cmd(int argc, char *argv[]);
char *realtime_off_cmd(int argc, char *argv[]);

void set_time(double);
double current_time(void);
void satellite_init(void);
void sats_init(void);
Constellation *get_constellation(void);
double get_central_body_radius(void);
unsigned int sats_update(void);

char *geomview_on_cmd(int argc, char *argv[]);
char *geomview_off_cmd(int argc, char *argv[]);
void geomview_set_flag(int flag);

char *sats_get_cmd(int argc, char *argv[]);
char *sats_set_cmd(int argc, char *argv[]);
char *sats_tag_cmd(int argc, char *argv[]);
char *sats_untag_cmd(int argc, char *argv[]);
char *sats_new_cmd(int argc, char *argv[]);
char *sats_copy_cmd(int argc, char *argv[]);
char *sats_delete_cmd(int argc, char *argv[]);
char *sats_delete_all_cmd(int argc, char *argv[]);
char *sats_load_marker_cmd(int argc, char *argv[]);
char *sats_load_cmd(int argc, char *argv[]);
char *sats_name_cmd(int argc, char *argv[]);
char *sats_get_name_cmd(int argc, char *argv[]);
char *sats_get_id_cmd(int argc, char *argv[]);

char *sats_debug_cmd(int argc, char *argv[]);

char *exit_cmd(int argc, char *argv[]);
char *reset_params_cmd(int argc, char *argv[]);
char *version_cmd(int argc, char *argv[]);
char *command_line_cmd(int argc, char *argv[]);

void gv_delayed_view_update(void);

#endif
/* !_SATS_H_ */
