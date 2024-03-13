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
 *
 * coverage.c
 *
 * $Id: coverage.c,v 1.47 2017/01/02 06:11:32 lloydwood Exp $
 */
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "sats.h"
#include "constants.h"
#include "globals.h"
#include "coverage_vis.h"
#include "stats_utils.h"
#include "savi.h"
#include "time.h"

static double START_TIME;

static int coverage_flag = FALSE;
static int tracks_flag = FALSE;
int no_access_flag = FALSE;
int map_flag = TRUE;
static int print_access_flag = FALSE;
static int project_tracks_flag = FALSE;
static int tracks_computed_flag = FALSE;
static int coverage_computed_flag = FALSE;

static unsigned int N_OBSERVATIONS = 1;
/* delta t (in seconds) for projecting ground tracks */
#define PROJECT_TRACKS_DT 5.0

/* if c_grid is NULL we are about to draw first image */
static grid *c_grid = NULL;

/*
 * redraw_coverage_display
 */
void
redraw_coverage_display(void)
{

  if (!c_grid) return;

  /* need to recompute coverage first */
  grid_and_foreground_to_image(coverage_projection, c_grid, TRUE);
  update_coverage_display();
}
/*
 * update_coverage_display
 */
void
update_coverage_display(void)
{
  if (!c_grid) return;

  update_display(coverage_projection, c_grid, coverage_flag,
		 tracks_flag, project_tracks_flag);
}

/*
 * coverage_on_cmd -
 * Set flags to indicate that coverage should be actually computed and
 * displayed.  Call coverage_compute and update_display to get the ball
 * rolling. (These are repeatedly called during the sats_update cycle.)
 */
char *
coverage_on_cmd(int argc, char *argv[])
{
  Constellation *pconstellation;
  CentralBody *pcb;
  Satellite_list sl;

  if (Image_Height * Image_Width <= 0) {
      return EMPTY_str;
  }

  /* do memory allocations and initialize display first time only */
  if (!c_grid) {
    c_grid = create_grid(Image_Height, Image_Width);
    image_init(c_grid);
  }

  /* set variables so that coverage will be computed and displayed */
  coverage_flag = TRUE;
  coverage_computed_flag = FALSE;

  init_coverage_time();		/* reset average coverage info */

  pconstellation = get_constellation();
  pcb = pconstellation->pcb;
  sl = pconstellation->satellites;

  coverage_compute(sl, FALSE, pcb);	/* compute the current coverage */
  update_coverage_display();

  return EMPTY_str;
}

char *
coverage_off_cmd(int argc, char *argv[])
{

  if (c_grid) {
    clear_interval(c_grid);
    clear_intensity(c_grid);
    update_coverage_display();
  }

  coverage_flag = FALSE;

  return EMPTY_str;
}


char *
coverage_set_cmd(int argc, char *argv[])
{
  Constellation *pconstellation = get_constellation();
  Satellite s;

  int n = atoi(argv[2]);
  int flag = atoi(argv[3]);

  s = constellation_search(pconstellation, n);
  if (s) {
    if (FALSE == flag) {
      s->can_display_coverage = 0;
    } else {
      s->can_display_coverage = 1;
    }
    if (geomview_module) {
      cones_gv_update();
      footprints_gv_update();
    }
  }

  return EMPTY_str;
}


char *
no_access_on_cmd(int argc, char *argv[])
{
  no_access_flag = TRUE;

  if (c_grid) {
    if (coverage_flag) {
      clear_interval(c_grid);
    }
    update_coverage_display();
  }

  return EMPTY_str;
}

char *
no_access_off_cmd(int argc, char *argv[])
{
  no_access_flag = FALSE;

  if (c_grid) {
    update_coverage_display();
  }

  return EMPTY_str;
}

char *
print_access_on_cmd(int argc, char *argv[])
{
  print_access_flag = TRUE;

  return EMPTY_str;
}

char *
print_access_off_cmd(int argc, char *argv[])
{
  print_access_flag = FALSE;

  return EMPTY_str;
}

char *
map_on_cmd(int argc, char *argv[])
{
  map_flag = TRUE;

  if (c_grid) {
    update_coverage_display();
  }

  redraw_earth(argc, argv);

  return EMPTY_str;
}

char *
map_off_cmd(int argc, char *argv[])
{
  map_flag = FALSE;

  if (c_grid) {
    update_coverage_display();
  }

  redraw_earth(argc, argv);

  return EMPTY_str;
}

char *
tracks_on_cmd(int argc, char *argv[])
{
  Constellation *pconstellation;
  CentralBody *pcb;
  Satellite_list sl;

  /* do memory allocations and initialize display first time only */
  if (!c_grid) {
    c_grid = create_grid(Image_Height, Image_Width);
    image_init(c_grid);
  }

  /* set variable so tracks will be plotted */
  tracks_flag = TRUE;
  tracks_computed_flag = FALSE;

  pconstellation = get_constellation();
  pcb = pconstellation->pcb;
  sl = pconstellation->satellites;

  tracks_compute(sl, pcb);	/* compute current tracks */
  update_coverage_display();

  return EMPTY_str;
}

char *
tracks_off_cmd(int argc, char *argv[])
{
  if (c_grid) {
    if (!project_tracks_flag) {
      reset_foreground(coverage_projection, c_grid);
    }
    update_coverage_display();
  }

  tracks_flag = FALSE;

  return EMPTY_str;
}

/*
 * project_tracks_on_cmd -
 * Draw ground tracks forwards in time for all satellites.
 */
char *
project_tracks_on_cmd(int argc, char *argv[])
{
  Constellation *pconstellation;
  Satellite_list sl, Sl;
  CentralBody *pcb;

  double c_time, f_time, t, dt; /* interval from hours to seconds */
  Satellite s;
  unsigned int color;
  int npts, npts_start;

  if (tracks_interval >= 0)
    dt = PROJECT_TRACKS_DT;
  else
    dt = -PROJECT_TRACKS_DT;

  c_time = current_time();
  f_time = c_time + tracks_interval * 60 * 60;  /* convert the tracks */
  npts_start = (int) ((f_time - c_time) / dt);

  pconstellation = get_constellation();
  pcb = pconstellation->pcb;
  sl = Sl = pconstellation->satellites;

  while (sl) {
    if (sl->s->can_display_satellite) {
      if (sl->s->tag == 1) {
        color = SPECIAL_PROJECT_TRACKS_CI;
      } else {
        color = PROJECT_TRACKS_CI;
      }
      s = satellite_create(0, &(sl->s->oe), pcb);
      t = c_time;
      npts = npts_start;
      while ((npts--) >= 0) {
        satellite_compute_position(s, t, pcb);
        track_to_foreground(s, coverage_projection, c_grid, color, pcb);
        t += dt;
      }
      satellite_destroy(s);
    }
    sl = sl->next;
  }
  project_tracks_flag = TRUE;

  if (c_grid) {
    update_coverage_display();
  }

  return EMPTY_str;
}

/*
 * project_tracks_off_cmd - Erase all ground tracks.
 */
char *
project_tracks_off_cmd(int argc, char *argv[])
{
  Constellation *pconstellation;
  Satellite_list sl;
  CentralBody *pcb;

  if (c_grid) {
    /* Rather than resetting entire foreground, we could
       step through it and selectively erase PROJECT_TRACKS_CI etc. */
    reset_foreground(coverage_projection, c_grid);

    if (tracks_flag) {
      pconstellation = get_constellation();
      pcb = pconstellation->pcb;
      sl = pconstellation->satellites;
      tracks_compute(sl, pcb);	/* compute current satellite locations */
    }
    update_coverage_display();
  }

  project_tracks_flag = FALSE;

  return EMPTY_str;
}

/*
 * project_tracks_reset
 */
void
project_tracks_reset()
{
  if (!project_tracks_flag)
    return;
  (void) project_tracks_off_cmd(0, NULL); /* clear current ground tracks */
  (void) project_tracks_on_cmd(0, NULL); /* and then re-draw. */
}

int
get_coverage_type()
{
  return coverage_type;
}

int
get_map_flag()
{
  return map_flag;
}

/*
 * Clear current coverage completely
 */
void
clear_coverage(void)
{
  if (c_grid) {
    clear_intensity(c_grid);
    clear_interval(c_grid);
  }
}

/*
 * Fade current coverage by increasing values
 */
void
coverage_decay(void)
{
  if (c_grid && coverage_flag && no_access_flag) {
    decay_interval(c_grid);
  }
}

/*
 * fill out the pixel map for the Tk image
 */
void
coverage_compute(const Satellite_list SL, unsigned int force,
		 const CentralBody * pcb)
{
  if (!coverage_flag && !force)
    return;

  if (!c_grid)
    return;

  clear_intensity(c_grid);

  fill_grid(SL, coverage_projection, coverage_type, coverage_angle * DEG_TO_RAD,
	    pcb, c_grid);
  coverage_computed_flag = TRUE;
  update_coverage_time();
}

/*
 * tracks_compute - Plots point under satellites onto foreground image
 */
void
tracks_compute(const Satellite_list SL, const CentralBody * pcb)
{
  if (!tracks_flag)
    return;

  if (!c_grid)
    return;

  if (!project_tracks_flag) {
    reset_foreground(coverage_projection, c_grid);
  }

  tracks_to_foreground(SL, coverage_projection, c_grid, GROUND_TRACKS_CI, pcb);
  tracks_computed_flag = TRUE;
}

/*
 * coverage_proj_cmd - sets type of projection
 */
char *
coverage_proj_cmd(int argc, char *argv[])
{
  Constellation *pconstellation = get_constellation();
  Satellite_list sl = pconstellation->satellites;
  CentralBody *pcb = pconstellation->pcb;

  coverage_compute(sl, FALSE, pcb);	/* compute the current coverage */
  if (c_grid) {
    update_coverage_display();
  }

  return EMPTY_str;
}

/*
 * coverage_angle_type -
 * sets type of angle for footprint computations - from earth or from satellite
 */
char *
coverage_angle_type_cmd(int argc, char *argv[])
{
  Constellation *pconstellation;
  Satellite_list sl;
  CentralBody *pcb;
  int t = atoi(argv[2]);

  coverage_type = t;

  pconstellation = get_constellation();
  sl = pconstellation->satellites;
  pcb = pconstellation->pcb;

  coverage_compute(sl, TRUE, pcb);	/* compute the current coverage */
  if (c_grid) {
    clear_interval(c_grid);
    update_coverage_display();
  }

  /* rebuild the coverage cones, footprints, .. */
  for (t = 0; t < N_VIEW_MODULES; t++)
    if (view_modules[t].coverage_rebuild_fn)
      view_modules[t].coverage_rebuild_fn();

  return EMPTY_str;
}

char *
coverage_save_map_cmd(int argc, char *argv[])
{
    write_image_to_file(argv[2]);
    return EMPTY_str;
}

char *
coverage_color_update_cmd(int argc, char *argv[])
{
  coverage_color_copy();
  return EMPTY_str;
}

/*
 * coverage_display - Display the coverage map.
 */
void
coverage_display(const Satellite_list SL, unsigned int force,
		 const CentralBody * pcb)
{
  if (!coverage_flag && !tracks_flag && !project_tracks_flag && !force)
    return;

  if (!c_grid)
    return;			/* image not yet created */

  update_coverage_display();
}

/*
 * init_coverage_time - Initialize variables for computing mean coverage.
 */
void
init_coverage_time()
{
  START_TIME = current_time();
  coverage_init_time_update(START_TIME);
  N_OBSERVATIONS = 1;
}

/*
 * update_coverage_time - Update coverage time, for use in computing mean coverage.
 */
void
update_coverage_time()
{
  coverage_time_update(current_time() - START_TIME);
}

/*
 * mean_coverage - Compute mean coverage over the current time interval.
 */
double
mean_coverage(int projection_type, grid * g)
{
  static double mean;

  mean =
    (mean * (N_OBSERVATIONS - 1) +
     total_coverage(projection_type, FALSE, g)) / N_OBSERVATIONS;
  N_OBSERVATIONS++;
  return (mean);
}


/*
 * Compute percentage covered from an intensity array.
 * Earth occupies 2/PI of the image if projection is SINUSOIDAL.
 * Earth occupies PI/4 of the image if the image is SPHERICAL.
 * And all of image if projection is CYLINDRICAL or UNPROJECTED.
 */
double
total_coverage(int projection_type, int output, grid *g)
{
  double cov;

  if (output) {
    if (print_access_flag) {
      float coverage, totCoverage = 0.0;

      /*
       * Use slightly off values to avoid cos(90), cos(-90)
       * because of later divide by 0
       */
      float lat = 89.85, latincr = 4.9905, cosLat;
      int i, numBins = 37;
      char coverStr[LENGTH_STRING_BUFFER] = "";

      /*
       * Format a string containing coverage percentages
       * for specified latitudes. Print the string
       */
      for (; lat > -89.99; lat -= latincr) {
        i = (90.0 - lat) * g->height / 180.0;
	switch (projection_type) {
	case SINUSOIDAL:
	case SPHERICAL:
	case SPHERICAL_90:
	  cosLat = cos(DEG_TO_RAD * lat);
	  break;
	default:
	  cosLat = 1.0;
	}

        if (debug) fprintf(stderr, "%.1f %d %d %d %d\n", lat, g->covered[i], (int) (cosLat * g->width), i, g->height);

        coverage = MIN(100.0, (100.0 * g->covered[i]) / (cosLat * g->width));

        /* Multiply coverage by cosLat to deemphasize poles and divide by numbins */
        totCoverage += (coverage * cosLat / numBins);
        sprintf(&coverStr[strlen(coverStr)], "%3.0f%s", coverage, ((lat > -85) ? " " : "\n"));
      }
      fprintf(stderr, coverStr);

      /* Use total coverage calculated by latitude instead of overall number */
      return MIN(100.0, (((projection_type == SINUSOIDAL)||
			  (projection_type == SPHERICAL) ||
			  (projection_type == SPHERICAL_90)) ?
			 totCoverage * PI / 2.0 : totCoverage));
    }
  }

  switch (projection_type) {
  case SINUSOIDAL:
    cov = 100.0*g->count*PI/g->height/g->width/2;
    break;
  case SPHERICAL:
  case SPHERICAL_90:
    cov = 100.0*g->count*4/g->height/g->width/PI;
    break;
  case CYLINDRICAL:
  case UNPROJECTED:
  default:
    /* is this correct for UNPROJECTED? */
    cov = 100.0*g->count/g->height/g->width;
  }

  return MIN(100.0, cov);
}
