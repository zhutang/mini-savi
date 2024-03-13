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
 * time.c
 *
 * $Id: time.c,v 1.20 2017/01/02 06:11:32 lloydwood Exp $
 */
#include <math.h>
#include <stdio.h>
#include <globals.h>

#include "time.h"

extern int tcl_script(char []);

static const int secs_in_minute = 60;
static const int secs_in_hour = 3600;
static const int secs_in_day = 86400;

/*
 * time_update
 *
 * Updates time on the screen
 *
 */
void
time_update(const double time)
{
  int days, hours, mins, tensecs;
  double secs=time;   /* time is in seconds */
  const char format[] = "set time \"Day %d  %.2d:%.2d:%.1d%.2f\"";

  days = (int) floor(secs/secs_in_day);
  secs = secs - days*secs_in_day;
  hours = (int) floor(secs/secs_in_hour);
  secs = secs - hours*secs_in_hour;
  mins = (int) floor(secs/secs_in_minute);
  secs = secs - mins*secs_in_minute;
  tensecs = (int) floor(secs/10);
  secs = secs - 10*tensecs;
  sprintf(cmd, format, days, hours, mins, tensecs, secs);
  tcl_script(cmd);
}

/*
 * coverage_time_update
 *
 * Updates time elapsed on the coverage screen
 *
 */
void
coverage_time_update(const double time)
{
  int days, hours, mins, tensecs;
  double secs=time;   /* time is in seconds */
  const char format[] = \
    "set coverage_time \"Elapsed time: %d days, %.2d:%.2d:%.1d%.2f\"";

  days = (int) floor(secs/secs_in_day);
  secs = secs - days*secs_in_day;
  hours = (int) floor(secs/secs_in_hour);
  secs = secs - hours*secs_in_hour;
  mins = (int) floor(secs/secs_in_minute);
  secs = secs - mins*secs_in_minute;
  tensecs = (int) floor(secs/10);
  secs = secs - 10*tensecs;
  sprintf(cmd, format, days, hours, mins, tensecs, secs);
  tcl_script(cmd);

}
/*
 * coverage_init_time_update
 *
 * Updates initial time on the coverage screen
 *
 */
void
coverage_init_time_update(const double time)
{
  int days, hours, mins, tensecs;
  double secs=time;   /* time is in seconds */
  const char format[] = \
    "set coverage_init_time \"Start time: Day %d  %.2d:%.2d:%.1d%.2f\"";

  days = (int) floor(secs/secs_in_day);
  secs = secs - days*secs_in_day;
  hours = (int) floor(secs/secs_in_hour);
  secs = secs - hours*secs_in_hour;
  mins = (int) floor(secs/secs_in_minute);
  secs = secs - mins*secs_in_minute;
  tensecs = (int) floor(secs/10);
  secs = secs - 10*tensecs;
  sprintf(cmd, format, days, hours, mins, tensecs, secs);
  tcl_script(cmd);

}
