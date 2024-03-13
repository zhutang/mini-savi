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
 * sats.c
 *
 * $Id: sats.c,v 1.61 2017/01/02 06:11:32 lloydwood Exp $
 */
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>

/* for select */
/*
#include <unistd.h>
#include <sys/types.h>
#include <bstring.h>
*/

#include "globals.h"
#include "constants.h"
#include "utils.h"
#include "coverage_vis.h"
#include "fisheye.h"
#include "gv_utils.h"
#include "Satellite.h"
#include "sats.h"
#include "stats_utils.h"
#include "savi.h"
#include "time.h"


unsigned int geomview_flag = FALSE;

static int current_marker = 0;

static double direction; /* 1.0 = FORWARDS, -1.0 = BACKWARDS */
/*
 * Central body parameter - global from the parameters panel.  It should
 * be initialized in the following routine so it can be reset at runtime
 */
static CentralBody CB = {
  EARTH_RADIUS,
  EARTH_ROTATION_RATE,
  EARTH_MU,
  EARTH_OBLATENESS,
  EARTH_YEAR,
  EARTH_TILT
};

static Constellation constellation = { 0, 0, 0, (Satellite_list) NULL, &CB };

static double ttime = 0.0;
static unsigned int realtime_flag = FALSE;
static double realtime = 0;

static Satellite sats_n(int n);
static char OK_str[] = "OK";

static const int Forward = 1.0;
static const int Backward = -1.0;

/*
 * forwards_cmd
 */
char *
forwards_cmd(int argc, char *argv[])
{
  if (!motion) {
    motion = TRUE;
    direction = Forward;
    set_realtime();
    return OK_str;
  }
  return EMPTY_str;
}

/*
 * backwards_cmd
 */
char *
backwards_cmd(int argc, char *argv[])
{
  if (!motion) {
    motion = TRUE;
    direction = Backward;
    set_realtime();
    return OK_str;
  }
  return EMPTY_str;
}

/*
 * forwards_step_cmd
 */
char *
forwards_step_cmd(int argc, char *argv[])
{
  if (!motion) {
    motion = TRUE;
    single_step = TRUE;
    direction = Forward;
    sats_update();		/* call twice to compute and move */
    sats_update();
    return OK_str;
  }
  return EMPTY_str;
}

/*
 * backwards_step_cmd
 */
char *
backwards_step_cmd(int argc, char *argv[])
{
  if (!motion) {
    motion = TRUE;
    single_step = TRUE;
    direction = Backward;
    sats_update();		/* call twice to compute and move */
    sats_update();
    return OK_str;
  }
  return EMPTY_str;
}

/*
 * reset_cmd
 */
char *
reset_cmd(int argc, char *argv[])
{
  reset = TRUE;
  return OK_str;
}


/*
 * stop_cmd
 */
char *
stop_cmd(int argc, char *argv[])
{
  if (motion) {
    sats_update();		/* call twice to compute and move */
    sats_update();
    motion = FALSE;
    return OK_str;
  }
  return EMPTY_str;
}


/*
 * set_realtime
 */
void
set_realtime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  realtime = tv.tv_sec + tv.tv_usec / 1000000.0;
}

/*
 * realtime_on_cmd
 */
char *
realtime_on_cmd(int argc, char *argv[])
{
  if (realtime_flag)
    return EMPTY_str;

  realtime_flag = TRUE;
  set_realtime();

  return EMPTY_str;
}

/*
 * realtime_off_cmd
 */
char *
realtime_off_cmd(int argc, char *argv[])
{
  if (!realtime_flag)
    return EMPTY_str;

  realtime_flag = FALSE;

  return EMPTY_str;
}

/*
 * geomview_on_cmd
 */
char *
geomview_on_cmd(int argc, char *argv[])
{

  if (!geomview_module)
    return EMPTY_str;
  geomview_set_flag(TRUE);

  /* plot all satellites */
  gv_begin();
  gv_trans_list_create(constellation.satellites);
  earth_place(ttime, constellation.pcb);
  sun_place(ttime, constellation.pcb);
  gv_end();

  return EMPTY_str;
}

/*
 * geomview_off
 */
char *
geomview_off_cmd(int argc, char *argv[])
{
  geomview_set_flag(FALSE);
  return EMPTY_str;
}

/*
 * geomview_set_flag(int flag)
 */
void
geomview_set_flag(int flag)
{
  geomview_flag = flag;
}

/*
 * set_time()
 *
 * Sets time variable and makes sure window displays it
 */
void
set_time(double t)
{
  ttime = t;
  time_update(t);
}

/*
 * current_time()
 *
 * Returns current value of time variable.
 */
double
current_time()
{
  return (ttime);
}

/*
 * get_constellation
 *
 * Returns a pointer to the constellation
 */
Constellation *
get_constellation()
{
  return &constellation;
}


/*
 * get_central_body_radius
 *
 */
double
get_central_body_radius()
{
  return constellation.pcb->radius;
}

/*
 * satellite_init
 *
 * Create initial example satellite
 */
void
satellite_init()
{
  Satellite s;

  OrbitalElements oe = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

  /* put at some fixed distance above the Earth */
  oe.a = 1.2 * get_central_body_radius();

  /* install one satellite */
  s = satellite_create(0, &oe, constellation.pcb);
  satellite_name(s, "example");
  constellation_add(&constellation, s);
  constellation_compute_positions(&constellation, ttime);

  /* display all satellites only if geomview is running */
  /* place first so that they are created in the correct positions */
  if (geomview_module) {
    gv_trans_list_create(constellation.satellites);
    gv_sat_list_create(&constellation);
  }
}

/*
 * sats_init
 *
 * Create initial configuration
 */
void
sats_init()
{
  /* set time */
  set_time(0.0);
}


/*
 * sats_update
 *
 * loop called repeatedly to update the configuration and
 * display of the satellites
 *
 */
unsigned int
sats_update()
{
  /*
   * Unit of time is second-dev-tes.
   */
  unsigned int i;
  static unsigned int computed = FALSE;
  static double computed_time = 0.0;
  static double old_delta_t = 0.0;
  static double old_ttime = 0.0;
  static int old_direction = 0;

  double oldt;

  if (reset) {
    set_time(0.0);
    computed_time = 0.0;
    init_coverage_time();
    computed = FALSE;
    reset = FALSE;

    constellation_compute_positions(&constellation, ttime);

    tracks_compute(constellation.satellites, constellation.pcb);
    fisheye_display(constellation.satellites, FALSE, constellation.pcb);

    clear_coverage();
    coverage_compute(constellation.satellites, FALSE, constellation.pcb);

    if (geomview_flag) {
      gv_start();
      if (transforms_needed) {
	gv_trans_list_create(constellation.satellites);
      }
      for (i = 0; i < N_VIEW_MODULES; i++) {
	if (view_modules[i].relocate_fn)
	  view_modules[i].relocate_fn(&constellation);
      }
      gv_delayed_view_update();
      earth_place(ttime, constellation.pcb);
      sun_place(ttime, constellation.pcb);
      /* updates display, sends scratchfile if necessary. */
      coverage_display(constellation.satellites, FALSE, constellation.pcb);
      gv_stop();
      gv_set_ready();
    } else {
      /* we're not talking to Geomview, but still need to draw coverage */
      coverage_display(constellation.satellites, FALSE, constellation.pcb);
    }

    project_tracks_reset();

  } else if (motion) {

    if (computed && !realtime_flag &&
	((old_ttime != ttime) ||
	 (old_delta_t != delta_t) ||
	 (old_direction != direction))) computed = FALSE;

    if (!computed) {

      if (!realtime_flag || single_step) {
	computed_time = ttime + direction * delta_t;
	old_ttime = ttime;
	old_delta_t = delta_t;
	old_direction = direction;
      } else {
	oldt = realtime;
	set_realtime();
	computed_time = ttime + direction * (realtime - oldt) * delta_t;
      }
      constellation_compute_positions(&constellation, computed_time);
      coverage_decay();
      coverage_compute(constellation.satellites, FALSE, constellation.pcb);
      tracks_compute(constellation.satellites, constellation.pcb);
      computed = TRUE;

      /* Pause briefly to avoid saturating the CPU. */
      millisleep(1);

    } else {

      if (!geomview_flag || gv_ready()) {
	set_time(computed_time);

	fisheye_display(constellation.satellites, FALSE, constellation.pcb);

	/* plot all satellites */
	if (geomview_flag) {
	  gv_start();
	  if (transforms_needed) {
	    gv_trans_list_create(constellation.satellites);
	  }

	  for (i = 0; i < N_VIEW_MODULES; i++) {
	    if (view_modules[i].relocate_fn)
	      view_modules[i].relocate_fn(&constellation);
	  }
	  gv_delayed_view_update();
	  earth_place(ttime, constellation.pcb);
	  sun_place(ttime, constellation.pcb);
	  /* updates display, sends scratchfile if necessary. */
	  coverage_display(constellation.satellites, FALSE, constellation.pcb);
	  gv_stop();
	  gv_set_ready();
	} else {
	  /* we're not talking to Geomview, but still need to draw coverage */
          coverage_display(constellation.satellites, FALSE, constellation.pcb);
        }

	if (single_step) {
	  single_step = FALSE;
	  motion = FALSE;
	}

	computed = FALSE;
      }
    }
  } else {
    /* If nothing's happening, pause 20 milliseconds to avoid
       saturating the CPU. */
    millisleep(20);
  }
  return TRUE;
}


/*
 * sats_new
 *
 * Adds a new satellite and makes it current
 */
char *
sats_new_cmd(int argc, char *argv[])
{
  Satellite s;
  OrbitalElements oe = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  static char result[10];

  oe.a = CB.radius * 1.2;

  s = satellite_create(0, &oe, constellation.pcb);
  constellation_add(&constellation, s);

  satellite_compute_position(s, ttime, constellation.pcb);
  if (geomview_module) {
    gv_begin();
    gv_trans_create(s);
    gv_sat_create(s, &constellation);
    gv_end();
  }

  sprintf(result, "%d", s->id);
  return result;
}

/*
 * sats_n
 *
 * Returns nth satellite (starting from 0)
 */
static Satellite
sats_n(int n)
{
  Satellite_list sl = constellation.satellites;

  while (sl && (n > 0)) {
    sl = sl->next;
    n--;
  }
  if (sl)
    return sl->s;

  return NULL;
}

/*
 * sats_get_cmd
 *
 * Returns orbital elements and properties of nth satellite in list.
 *
 * Don't forget: User interface angles are in degrees, but internally we
 * like radians.
 *
 */
char *
sats_get_cmd(int argc, char *argv[])
{
  int n = atoi(argv[2]);
  Satellite s = sats_n(n);


  if (s) {
    static char result[300];
    char altitude[15];

    int id = s->id;

    if (id < 10) {
      sprintf(altitude,"%12.2f", s->oe.a);
    } else if (id < 100) {
      sprintf(altitude,"%11.2f", s->oe.a);
    } else if (id < 1000) {
      sprintf(altitude,"%10.2f", s->oe.a);
    } else {
      sprintf(altitude,"%9.2f", s->oe.a);
    }

    sprintf(result,
	    "%s %10.4f %8.3f %12.3f %12.3f %13.3f "
	    "%10.3f %10.4f %10.4f %10.2f %10.2f %d %d %d",
	    altitude,
	    s->oe.e,
	    s->oe.i * RAD_TO_DEG,
	    s->oe.Omega * RAD_TO_DEG,
	    s->oe.omega * RAD_TO_DEG,
	    s->oe.T,
	    oe_to_period(&(s->oe), constellation.pcb) / 60,
	    oe_to_nodal_precession(&(s->oe),
		     constellation.pcb) * RAD_TO_DEG * SECS_PER_DAY,
	    oe_to_apsidal_rotation(&(s->oe),
		     constellation.pcb) * RAD_TO_DEG * SECS_PER_DAY,
	    oe_to_apoapsis_altitude(&(s->oe),
		     constellation.pcb),
	    oe_to_periapsis_altitude(&(s->oe), constellation.pcb),
	    s->can_display_satellite, s->can_display_orbit,
	    s->can_display_coverage);

    return result;
  }
  return EMPTY_str;
}

/*
 * sats_set_cmd
 *
 * Sets the orbital elements of a particular satellite.
 *
 * Don't forget: User interface angles are in degrees, but internally we
 * like radians.
 */
char *
sats_set_cmd(int argc, char *argv[])
{
  unsigned int j;
  int n = atoi(argv[2]);
  Constellation *pconstellation = get_constellation();

  Satellite s = sats_n(n);

  if (s) {
    float a, e, i, o, u, t;

    /* should sanity-check and bound all parameters */
    /* semi-major axis greater than 0 */
    a = atof(argv[3]);
    /* negative altitudes turned into positive */
    if (a < 0) {
      a = -a;
      if (debug) error("turned negative semi-major axis into positive");
    }
    /* check altitude is not zero to avoid /0 */
    if (a == 0) a = 0.0001;

    /* eccentricity between 0 and 1 */
    e = atof(argv[4]);
    if (e < 0) {
      e = -e;
      if (debug) error("turned negative eccentricity into positive");
    }
    if (e > 0.999) e = 0.999;

    i = atof(argv[5]) * DEG_TO_RAD;
    o = atof(argv[6]) * DEG_TO_RAD;
    u = atof(argv[7]) * DEG_TO_RAD;
    t = atof(argv[8]);

    s->oe.a = a;
    s->oe.e = e;
    s->oe.i = i;
    s->oe.Omega = o;
    s->oe.omega = u;
    s->oe.T = t;

    satellite_oe_edited(s, pconstellation->pcb);

    if (atoi(argv[9])) {
      /* allow satellite to be viewed */
      if (!s->can_display_satellite) {
	s->can_display_satellite = 1;
	pconstellation->n_view_sats++;
      }
    } else {
      /* don't allow satellite to be viewed */
      if (s->can_display_satellite) {
	s->can_display_satellite = 0;
	pconstellation->n_view_sats--;
      }
    }
    s->can_display_satellite = atoi(argv[9]) ? 1 : 0;
    s->can_display_orbit = atoi(argv[10]) ? 1 : 0;
    s->can_display_coverage = atoi(argv[11]) ? 1 : 0;

    satellite_compute_position(s, ttime, pconstellation->pcb);
    if (geomview_module) {
      gv_begin();
      gv_trans_create(s);
      for (j = 0; j < N_VIEW_MODULES; j++)
	if (view_modules[j].display_fn)
	  view_modules[j].display_fn(s, pconstellation);
      gv_end();
    }
  }
  return EMPTY_str;
}

/*
 * sats_tag_cmd
 *
 * Sets the nth satellite as distinguished.
 */
char *
sats_tag_cmd(int argc, char *argv[])
{
  int n = atoi(argv[2]);
  Satellite s = sats_n(n);

  if (s && !s->tag) {
    s->tag = 1;
   (constellation.n_tag_sats)++;
   if (debug) fprintf(stderr,"SaVi: after tagging, tagged %i satellites.",constellation.n_tag_sats);
    if (geomview_module) {
      gv_begin();
      gv_sat_create(s, &constellation);
      gv_end();
    }
  }

  return EMPTY_str;
}

/*
 * sats_untag_cmd
 *
 * Untags the nth satellite
 */
char *
sats_untag_cmd(int argc, char *argv[])
{
  int n = atoi(argv[2]);
  Satellite s = sats_n(n);

  if (s && s->tag) {
    s->tag = 0;
    (constellation.n_tag_sats)--;
    if (debug) fprintf(stderr,"SaVi: after untagging, tagged %i satellites.",constellation.n_tag_sats);
    if (geomview_module) {
      gv_begin();
      gv_sat_create(s, &constellation);
      gv_end();
    }
  }

  return EMPTY_str;
}

/*
 * sats_delete_cmd
 *
 * Deletes satellite n
 */
char *
sats_delete_cmd(int argc, char *argv[])
{
  int n = atoi(argv[2]);
  int i;
  Satellite s = sats_n(n);

  if (s) {
    constellation_destroy_element(&constellation, s->id);
    if (geomview_module) {
      gv_begin();
      for (i = 0; i < N_VIEW_MODULES; i++)
	if (view_modules[i].delete_fn)
	  view_modules[i].delete_fn(s);
      gv_end();
    }
  }
  return EMPTY_str;
}

/*
 * sats_copy_cmd
 *
 */
char *
sats_copy_cmd(int argc, char *argv[])
{
  static char result[10];
  int n = atoi(argv[2]);
  Satellite old_s = sats_n(n);
  Satellite s = old_s;

  if (old_s) {
    s = satellite_create(0, &(old_s->oe), constellation.pcb);
    satellite_name(s, old_s->name);
    constellation_add(&constellation, s);
    satellite_compute_position(s, ttime, constellation.pcb);
    if (geomview_module) {
      gv_begin();
      gv_trans_create(s);
      gv_sat_create(s, &constellation);
      gv_end();
    }
  }

  if (s) {
    sprintf(result, "%d", s->id);
  }

  return result;
}

/*
 * sats_load_marker_cmd
 */
char *
sats_load_marker_cmd(int argc, char *argv[])
{

  double a = atof(argv[2]);
  double Omega = atof(argv[3]);
  double omega = atof(argv[4]);
  double T = atof(argv[5]);
  char* name = argv[6];

  Satellite s;
  int n;
  static char result[20];

  OrbitalElements oe;

  oe.a = a;
  oe.e = 0;
  oe.i = 90 * DEG_TO_RAD; /* N/S for drawing vertical lines */
  oe.Omega = Omega * DEG_TO_RAD;
  oe.omega = omega * DEG_TO_RAD;
  oe.T = T;

  if (current_marker >= EQUATORIAL_MARKERS) {
    current_marker = 0;
    if (debug) {
      error("marker creation wrapped round array - redrawing?");
    }
  }
  
  marker[current_marker] = satellite_create(0, &oe, constellation.pcb);
  if (name && !satellite_name(marker[current_marker], name)) {
    error ("problem naming marker.");
  }
  satellite_compute_position(marker[current_marker], 0, constellation.pcb);

  current_marker++;
  
  return result;
}

/*
 * sats_load_cmd
 *
 * Don't forget: User interface angles are in degrees, but internally we
 * like radians.
 */
char *
sats_load_cmd(int argc, char *argv[])
{
  double a = atof(argv[2]);
  double e = atof(argv[3]);
  double i = atof(argv[4]);
  double Omega = atof(argv[5]);
  double omega = atof(argv[6]);
  double T = atof(argv[7]);
  char* name = argv[8];

  Satellite s;
  int n;
  static char result[20];

  OrbitalElements oe;

  oe.a = a;
  oe.e = e;
  oe.i = i * DEG_TO_RAD;
  oe.Omega = Omega * DEG_TO_RAD;
  oe.omega = omega * DEG_TO_RAD;
  oe.T = T;

  s = satellite_create(0, &oe, constellation.pcb);

  if (name && !satellite_name(s, name)) error ("problem naming satellite.");

  n = constellation_add(&constellation, s);
  satellite_compute_position(s, ttime, constellation.pcb);

  if (geomview_module) {
    gv_start();
    gv_trans_create(s);
    gv_sat_create(s, &constellation);
    gv_stop();
  }

  sprintf(result, "%d", n);
  return result;
}

/*
 * sats_name_cmd
 *
 * Names the given satellite
 */
char *
sats_name_cmd(int argc, char *argv[])
{
  Satellite s;
  int n;

  char *name = argv[3];

  n = atoi(argv[2]);

  s = sats_n(n);

  if (s) satellite_name(s, name);

  return EMPTY_str;
}

/*
 * sats_get_name_cmd
 *
 * Gets name of the given satellite
 */
char *
sats_get_name_cmd(int argc, char *argv[])
{
  Satellite s;
  unsigned int n;
  int i;
  static char result[SATELLITE_NAME_LENGTH] = "                        ";

  i = atoi(argv[2]);
  s = sats_n(i);

  if (s) {
    n = strlen((char *)&s->name);
    if (n > SATELLITE_NAME_LENGTH) {
      error("string has overflowed in struct");
      return EMPTY_str;
    }
    strcpy(result, (char *) &s->name);

    /* terminate string - elements are 0 to length-1 */
    result[SATELLITE_NAME_LENGTH - 1] = 0;
  }

  return result;

}

/*
 * sats_get_id_cmd
 *
 * Gets id no of the given satellite
 * Needed because the id # may not be same as list position,
 * if e.g. you delete a satellite.
 */
char *
sats_get_id_cmd(int argc, char *argv[])
{
  Satellite s;
  int n;
  static char result[12] = "            ";

  n = atoi(argv[2]);
  s = sats_n(n);

  if (s) {
    sprintf(result, "%d", s->id);
  } else {
    sprintf(result, "%d", NOT_OK);
  }

  /* terminate the string - 12 long, but 0 to 11. */
  result[11] = 0;

  return result;

}



/*
 * sats_delete_all_cmd
 *
 * Deletes all satellites in the satellite list
 */
char *
sats_delete_all_cmd(int argc, char *argv[])
{
  unsigned int i;

  while (constellation.satellites)
    constellation_destroy_element(&constellation,
				  constellation.satellites->s->id);

  if (geomview_module) {
    gv_start();
    for (i = 0; i < N_VIEW_MODULES; i++)
      if (view_modules[i].gv_delete_fn)
	view_modules[i].gv_delete_fn();
    gv_stop();
  }

  return EMPTY_str;
}


/*
 * sats_debug_cmd
 *
 * Dumps satellite list to stderr
 */
char *
sats_debug_cmd(int argc, char *argv[])
{
  Satellite_list sl = constellation.satellites;

  fprintf(stderr, "Satellites:\n");
  while (sl) {
    fprintf(stderr, "  %d %g %g %g %g %g %g",
	    sl->s->id, sl->s->oe.a, sl->s->oe.e, sl->s->oe.i,
	    sl->s->oe.Omega, sl->s->oe.omega, sl->s->oe.T);
    fprintf(stderr, "\n");
    sl = sl->next;
  }
  return EMPTY_str;
}

/*
 * reset_params_cmd
 *
 * Resets values of central body
 */
char *
reset_params_cmd(int argc, char *argv[])
{
  CentralBody *pcb = constellation.pcb;

  pcb->radius = EARTH_RADIUS;
  pcb->rotation_rate = EARTH_ROTATION_RATE;
  pcb->mu = EARTH_MU;
  pcb->J2 = EARTH_OBLATENESS;
  pcb->orbital_period = EARTH_YEAR;
  pcb->tilt = EARTH_TILT;

  return EMPTY_str;
}


/*
 * exit_cmd
 *
 * Clean up everything
 */
char *
exit_cmd(int argc, char *argv[])
{
  unsigned int i;

  (void) sats_delete_all_cmd(argc, argv);

  if (!geomview_module)
    return EMPTY_str;

  gv_start();
  for (i = 0; i < N_VIEW_MODULES; i++) {
    if (view_modules[i].off_cmd) {
      (void) view_modules[i].off_cmd(argc, argv);
    }
  }

  gv_stop();

  coverage_dynamic_cleanup();

  return EMPTY_str;
}


void
gv_delayed_view_update()
{
  Constellation *pconstellation;
  unsigned int i;

  if (!geomview_module)
    return;

  pconstellation = get_constellation();
  for (i = 0; i < N_VIEW_MODULES; i++)
    if (view_modules[i].write_geom_fn)
      view_modules[i].write_geom_fn(pconstellation);
}


/*
 * Return the version string
 */
char *
version_cmd(int argc, char *argv[])
{
  return Version;
}

/*
 * Return the list of valid command line parameters
 */
char *
command_line_cmd(int argc, char *argv[])
{
  return command_switches;
}
