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
 * fisheye.c
 *
 * $Id: fisheye.c,v 1.62 2017/01/02 06:11:32 lloydwood Exp $
 */

#include <math.h>
#include <stdlib.h>

#include "constants.h"
#include "globals.h"
#include "sats.h"
#include "savi.h"
#include "tcl_utils.h"

#include "fisheye.h"

static unsigned int fisheye_flag = FALSE;
static unsigned int fisheye_labels_flag = FALSE;
static unsigned int fisheye_reverse_view_flag = FALSE;
static unsigned int fisheye_print_inview_flag = FALSE;
static unsigned int fisheye_names_flag = FALSE;
static unsigned int fisheye_numbers_flag = FALSE;
static unsigned int fisheye_times_flag = FALSE;

static char plotStr[LENGTH_STRING_BUFFER];
static char fisheye_widget[] = ".fisheye.cmd.p";
static char font[] = "helvetica 9";

static void fisheye_location_string(char * string);
static void fisheye_update(void);

double lat, lon;

/*
 * Provide current fisheye coordinates for use elsewhere.
 */

void
fisheye_location(LatLon * l)
{
    l->lat = lat;
    l->lon = lon;
}

/*
 * Print lat/long coords in string ready to show to user.
 */
static void
fisheye_location_string(char * plotStr)
{
  char latNS, lonEW;
  double lat_min, lon_min;
  int lat_deg, lon_deg;

  /* display lat lon */
  if (lat >= 0) {
    latNS = 'N';
    lat_deg = (int) floor(lat);
    lat_min = (lat - lat_deg) * 60.0;
  } else {
    latNS = 'S';
    lat_deg = (int) floor(-lat);
    lat_min = (-lat - lat_deg) * 60.0;
  }
  if (lon >= 0) {
    lonEW = 'E';
    lon_deg = (int) floor(lon);
    lon_min = (lon - lon_deg) * 60.0;
  } else {
    lonEW = 'W';
    lon_deg = (int) floor(-lon);
    lon_min = (-lon - lon_deg) * 60.0;
  }

  sprintf(plotStr, "%i %2.0f' %c %i %2.0f' %c", lat_deg, lat_min, latNS, lon_deg, lon_min, lonEW);
}

/*
 * fisheye_update
 *
 * called if we need to redraw the fisheye because of
 * a user change.
 */
static void
fisheye_update(void) {
  Constellation *pconstellation;
  Satellite_list SL;
  CentralBody *pcb;

  if (fisheye_flag) {
    pconstellation = get_constellation();
    SL = pconstellation->satellites;
    pcb = pconstellation->pcb;

    /* display the fisheye view */
    fisheye_display(SL, TRUE, pcb);
  }
}

/*
 * fisheye_on
 *
 * Set flags to indicate that fisheye should be displayed.
 * Call fisheye_display to get updated display.
 */
char *
fisheye_on_cmd(int argc, char *argv[])
{
  /* set flag so that fisheye will be displayed */
  fisheye_flag = TRUE;

  /* display the fisheye view */
  fisheye_update();

  return EMPTY_str;
}

char *
fisheye_off_cmd(int argc, char *argv[])
{
  unsigned int old_flag = fisheye_flag;

  fisheye_flag = FALSE;

  if (fisheye_flag != old_flag) fisheye_update();

  return EMPTY_str;
}

char *
fisheye_labels_on_cmd(int argc, char *argv[])
{
  unsigned int old_flag = fisheye_labels_flag;

  fisheye_labels_flag = TRUE;

  if (fisheye_labels_flag != old_flag) fisheye_update();

  return EMPTY_str;
}

char *
fisheye_labels_off_cmd(int argc, char *argv[])
{
  unsigned int old_flag = fisheye_labels_flag;

  fisheye_labels_flag = FALSE;

  if (fisheye_labels_flag != old_flag) fisheye_update();

  return EMPTY_str;
}

char *
fisheye_reverse_view_on_cmd(int argc, char *argv[])
{
  unsigned int old_flag = fisheye_reverse_view_flag;

  fisheye_reverse_view_flag = TRUE;

  /* display the fisheye view */
  if (fisheye_reverse_view_flag != old_flag) fisheye_update();

  return EMPTY_str;
}

char *
fisheye_reverse_view_off_cmd(int argc, char *argv[])
{
  unsigned int old_flag = fisheye_reverse_view_flag;

  fisheye_reverse_view_flag = FALSE;

  /* display the fisheye view */
  if (fisheye_reverse_view_flag != old_flag) fisheye_update();

  return EMPTY_str;
}

char *
fisheye_print_inview_on_cmd(int argc, char *argv[])
{
  fisheye_print_inview_flag = TRUE;

  return EMPTY_str;
}

char *
fisheye_print_inview_off_cmd(int argc, char *argv[])
{
  fisheye_print_inview_flag = FALSE;

  /* could delete or hide mask elevation circle */

  return EMPTY_str;
}

char *
fisheye_names_on_cmd(int argc, char *argv[])
{
  unsigned int old_flag = fisheye_names_flag;

  fisheye_names_flag = TRUE;

  if (fisheye_names_flag != old_flag) fisheye_update();

  return EMPTY_str;
}

char *
fisheye_names_off_cmd(int argc, char *argv[])
{
  unsigned int old_flag = fisheye_names_flag;

  fisheye_names_flag = FALSE;

  if (fisheye_names_flag != old_flag) fisheye_update();

  return EMPTY_str;
}

char *
fisheye_numbers_on_cmd(int argc, char *argv[])
{
  unsigned int old_flag = fisheye_numbers_flag;

  fisheye_numbers_flag = TRUE;

  if (fisheye_numbers_flag != old_flag) fisheye_update();

  return EMPTY_str;
}

char *
fisheye_numbers_off_cmd(int argc, char *argv[])
{
  unsigned int old_flag = fisheye_numbers_flag;

  fisheye_numbers_flag = FALSE;

  if (fisheye_numbers_flag != old_flag) fisheye_update();

  return EMPTY_str;
}

char *
fisheye_times_on_cmd(int argc, char *argv[])
{
  unsigned int old_flag = fisheye_times_flag;

  fisheye_times_flag = TRUE;

  if (fisheye_times_flag != old_flag) fisheye_update();

  return EMPTY_str;
}

char *
fisheye_times_off_cmd(int argc, char *argv[])
{
  unsigned int old_flag = fisheye_times_flag;

  fisheye_times_flag = FALSE;

  if (fisheye_times_flag != old_flag) fisheye_update();

  return EMPTY_str;
}

char *
fisheye_viewpoint_on_cmd(int argc, char *argv[])
{
  fisheye_viewpoint_flag = TRUE;

  update_coverage_display();

  return EMPTY_str;
}

char *
fisheye_viewpoint_off_cmd(int argc, char *argv[])
{
  fisheye_viewpoint_flag = FALSE;

  /* can't seem to remove cross - update_ and redraw_coverage_display() don't work. */

  return EMPTY_str;
}

/*
 * fisheye_set
 *
 * Sets the lat and lon for the fisheye display and redisplays it
 */
char *
fisheye_set_cmd(int argc, char *argv[])
{
  Constellation *pconstellation = get_constellation();
  Satellite_list sl = pconstellation->satellites;
  CentralBody *pcb = pconstellation->pcb;
  double new_lat = (double) atoi(argv[2]);
  double new_lon = (double) atoi(argv[4]);	/* was 3 gjl */

  new_lat = (new_lat >= 0) ? new_lat + atof(argv[3]) / 60.0 :
    new_lat - atof(argv[3]) / 60.0;
  new_lon = (new_lon >= 0) ? new_lon + atof(argv[5]) / 60.0 :
    new_lon - atof(argv[5]) / 60.0;

  lat = new_lat;
  lon = new_lon;

  fisheye_location_string(plotStr);

  sprintf(cmd, "set fisheye_lat_lon \"%s\"", plotStr);

  tcl_script(cmd);

  /* display sky view */
  if (fisheye_flag) fisheye_display(sl, TRUE, pcb);
  return EMPTY_str;
}

/*
 * fisheye_display
 *
 * update the Tk image
 */
void
fisheye_display(const Satellite_list SL, unsigned int force,
		const CentralBody * pcb)
{
  int offset, test, not_visible_to_terminal;
  static char attributes[30];

  float coverage_pct = (90.0 - coverage_angle) / 90.0;

  if (!fisheye_flag && !force)
    return;

  sprintf(cmd, "fisheye(delete)");
  test = tcl_script(cmd);
  if (debug && !test) {
    error("Problem telling Tk to delete fisheye.");
  }

  /* plot satellite positions */
  {
    CartesianCoordinates ccs;
    double sky[2];
    double t = current_time();
    double interval_time;
    Satellite_list sl = SL;

    unsigned int sats_visible, sats_above_mask;
    int a, d, n, r, x, y, x_start, y_start, x_end, y_end;
    int adjust = 2;
    int mirror = -1;

    sprintf(cmd,"fisheye(resize)");
    test = tcl_script(cmd);
    if (debug && !test) {
      error("problem telling Tk to resize fisheye disk.");
    }

    d = Fisheye_Diameter - adjust;
    r = (float)(d/2) - 1;
    offset = r + adjust; /* important for later */

    if (coverage_type == MASK_ELEVATION) {
      x_start = (float) (d * coverage_angle / 180) + adjust;
      y_end = d - x_start;
      sprintf(cmd,"%s create oval %i %i %i %i -outline red -width 1 -tags mask_elevation", fisheye_widget, x_start, x_start, y_end, y_end);
      test = tcl_script(cmd);
      if (debug && !test) {
        error("Problem telling Tk to draw mask elevation.");
      }
    }

    sats_visible = sats_above_mask = 0;

    if (fisheye_reverse_view_flag) {
      sprintf(attributes, "-anchor ne -fill blue");
    } else {
      sprintf(attributes, "-anchor nw -fill blue");
    }

    /* display equatorial ring markers, if turned on. Unmoving, time 0. */
    if (plane_flag) {
      sprintf(plotStr, "purple");
      for (n = 0; n < EQUATORIAL_MARKERS; n++) {
      
        oe_time_to_geocentric(&ccs, 0, &(marker[n]->oe), pcb);
  
        if (sat_to_fisheye(&ccs, lat, lon, pcb, 0, sky)) {
	  if (fisheye_reverse_view_flag) {
	    sky[0] = -sky[0];
	  }
	  
          x = (int) (offset + r * sky[0]);
	  y = (int) (offset - r * sky[1]); /* North is top of canvas */

	  /* Draw parallels in dark gray. Plain gray is too light on a Mac. */
	  if (n > 359) {
	    sprintf(plotStr, "darkgray");
	    if (n > 719) {
	      mirror = -mirror;
	    }
	  }
	  
	  sprintf(cmd, "%s create line %i %i %i %i -fill %s -tags equator",
                    fisheye_widget, x, y, x, y+mirror, plotStr);
	  test = tcl_script(cmd);
        }
      }
    }

    if (debug && !test) {
      error("Problem telling Tk to draw equator markers.");
    }
    
    /* display satellites */
    while (sl) {

      if (!sl->s->can_display_satellite) {
	sl = sl->next;
	continue;
      }

      oe_time_to_geocentric(&ccs, t, &(sl->s->oe), pcb);
      if (sat_to_fisheye(&ccs, lat, lon, pcb, t, sky)) {
	
        sats_visible++;
	
	if ((sky[0] * sky[0] + sky[1] * sky[1]) <
	    (coverage_pct * coverage_pct)) {
	  if (sl->s->gs_into_view == 0) {
	    /* fudge time slightly so that recorded value is never zero. */
	    sl->s->gs_into_view = t - 0.00001;
	    sl->s->gs_last_into_view = t - 0.00001;
	  }
	} else {
	  /*
	   * could add delta_t for half of interval before and after
	   * passing view test - but assumes delta_t unchanged!
	   */
	  if (sl->s->gs_into_view != 0) {
	    sl->s->gs_last_pass = t - sl->s->gs_into_view;
	  }
	  sl->s->gs_into_view = 0;
	}

	if (fisheye_reverse_view_flag) {
	  sky[0] = -sky[0];
	}

	not_visible_to_terminal = ((coverage_type == MASK_ELEVATION) &&
	                          (!sl->s->gs_into_view)) ||
	                          !sl->s->can_display_coverage;
	a = sl->s->x_S.r - pcb->radius;
	if (a <= min_transmit_altitude) {
	  not_visible_to_terminal = TRUE;
	}


	if (not_visible_to_terminal || (coverage_type != MASK_ELEVATION)) {
	  if (fisheye_names_flag) {
	    if (fisheye_numbers_flag) {
	      sprintf(plotStr, "{%d %s}", sl->s->id, sl->s->name);
	    } else {
	      sprintf(plotStr, "{%s}", sl->s->name);
	    }
	  } else {
	    if (fisheye_numbers_flag) {
	      sprintf(plotStr, "{%d}", sl->s->id);
	    } else {
	      /* print nothing */
	      sprintf(plotStr, "{}");
	    }
	  }
	} else {
	  sats_above_mask++;
	  interval_time = t - sl->s->gs_into_view;
	  if (fisheye_times_flag) {
	    if (fisheye_names_flag) {
	      if (fisheye_numbers_flag) {
		sprintf(plotStr, "{%d %s %.1f}", sl->s->id, sl->s->name, interval_time);
	      } else {
		sprintf(plotStr, "{%s %.1f}", sl->s->name, interval_time);
	      }
	    } else {
	      if (fisheye_numbers_flag) {
	        sprintf(plotStr, "{%d %.1f}", sl->s->id, interval_time);
	      } else {
		sprintf(plotStr, "{%.1f}", interval_time);
	      }
	    }
	  } else {
	    if (fisheye_names_flag) {
	      if (fisheye_numbers_flag) {
	        sprintf(plotStr, "{%d %s}", sl->s->id, sl->s->name);
	      } else {
	        sprintf(plotStr, "{%s}", sl->s->name);
	      }
	    } else {
	      if (fisheye_numbers_flag) {
	        sprintf(plotStr, "{%d}", sl->s->id);
	      } else {
		/* print nothing */
		sprintf(plotStr, "{}");
	      }
	    }
	  }
	}

	x = (int) (offset + r * sky[0]);
	y = (int) (offset - r * sky[1]); /* North is top of canvas */

	if (fisheye_labels_flag) {
	  sprintf(cmd, "%s create text %i %i -text %s %s -font {%s} -tags labels",
		  fisheye_widget, x, y, plotStr, attributes, font);
	  test = tcl_script(cmd);
	  if (debug && !test) {
	    error("Problem telling Tk to write fisheye widget satellite labels.");
	  }
	}

	if (sl->s->tag == 1) {
	  sprintf(plotStr, "pink");
	} else if (not_visible_to_terminal) {
	  sprintf(plotStr, "red");
	} else {
	  sprintf(plotStr, "green");
	}

	x_start = x-3;
	x_end = x+4;
	y_start = y-3;
	y_end = y+4;

	sprintf(cmd, "%s create line %i %i %i %i -fill %s -tags crosses",
		fisheye_widget, x, y_start, x, y_end, plotStr);
	test = tcl_script(cmd);
	sprintf(cmd, "%s create line %i %i %i %i -fill %s -tags crosses",
                fisheye_widget, x_start, y, x_end, y, plotStr);
	test = test && tcl_script(cmd);
	
	if (debug && !test) {
	  error("Problem telling Tk to draw on fisheye.");
	}
	
      } else {
	sl->s->gs_into_view = 0;
	/*
	 * any recorded output needs formatting into a proper tracefile,
	 * including lat/long, estimated margin of error (delta_t/2).
	 * currently disabled.
	 */
	if (sl->s->gs_last_pass != 0) {
	  if (0) {
	    fprintf(stderr,
		    "Sat: %2d, first seen: %6.1f secs, for: %.1f secs.\n",
		    sl->s->id, sl->s->gs_last_into_view, sl->s->gs_last_pass);
	  }
	}
      }

      sl = sl->next;
    }

    /* if canvas is really too small, don't write in corners. */
    if (d < 75) return;

    if (sats_above_mask) {
      /* output number of satellites */
      sprintf(plotStr, "%i in mask", sats_above_mask);
      sprintf(cmd, "%s create text %i %i -text {%s} -anchor ne -font {%s} -fill green -tags labels",
	      fisheye_widget, d-2, 2, plotStr, font);
      test = test && tcl_script(cmd);
    }

    /* output number of satellites visible */
    if (d < 175) {
      if ((coverage_type != MASK_ELEVATION) || !sats_above_mask) {
        sprintf(plotStr, "-text {%i in view} -anchor ne", sats_visible);
      } else {
	sprintf(plotStr, " ");
      }
      y_start = 2;
    } else {
      sprintf(plotStr, "-text {%i in view} -anchor se", sats_visible);
      y_start = d-2;
    }

    if (sats_visible) {
      sprintf(cmd, "%s create text %i %i %s -font {%s} -fill blue -tags labels",
	      fisheye_widget, d-2, y_start, plotStr, font);
      test = test && tcl_script(cmd);
    }

    /* output coordinates */
    fisheye_location_string(plotStr);
    sprintf(cmd, "%s create text %i %i -text {%s} -anchor sw -font {%s} -fill blue -tags labels",
	    fisheye_widget, 2, d-2, plotStr, font);
    test = test && tcl_script(cmd);

    if (debug && !test) {
      error("Problem telling Tk to write satellite counts.");
    }
  }
}
