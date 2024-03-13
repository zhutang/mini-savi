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
 * satellites.c
 *
 * routines for satellites in Geomview
 *
 * $Id: satellites.c,v 1.27 2017/01/02 06:11:32 lloydwood Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "int_types.h"
#include "globals.h"
#include "constants.h"
#include "gv_file.h"
#include "gv_utils.h"
#include "sats.h"
#include "orbit_utils.h"
#include "savi.h"

static int satellites_on_flag = FALSE;
static int satellites_geom_exists = FALSE;
static int satellites_geom_needs_updating = FALSE;

static int use_box_satellite = FALSE;
static int use_planes_satellite = FALSE;
static int use_fancy_satellite = FALSE;
static int use_sphere_satellite = FALSE;
static int use_fast_marker_satellite = TRUE;

static char box_hname[] = "box_satellite_h";
static char box_special_hname[] = "box_special_satellite_h";
static char planes_hname[] = "planes_satellite_h";
static char planes_special_hname[] = "planes_special_satellite_h";
static char fancy_hname[] = "fancy_satellite_h";
static char fancy_special_hname[] = "fancy_special_satellite_h";
static char sphere_hname[] = "sphere_satellite_h";
static char sphere_special_hname[] = "sphere_special_satellite_h";

/*
 * routine to display satellites
 */
char *
satellites_on_cmd(int argc, char *argv[])
{
  if (satellites_on_flag)
    return EMPTY_str;
  satellites_on_flag = TRUE;
  satellites_geom_needs_updating = TRUE;

  gv_start();

  if (use_box_satellite || use_planes_satellite || use_fancy_satellite) {
    transforms_needed |= (1 << SATELLITES);
    /* and send them too */
    gv_trans_list_create(get_constellation()->satellites);
  }

  write_satellites_geom(get_constellation());
  gv_stop();

  return EMPTY_str;
}

/*
 * satellites_off_cmd()
 *
 * Routine to turn off display of satellites
 */
char *
satellites_off_cmd(int argc, char *argv[])
{
  if (!satellites_on_flag)
    return EMPTY_str;

  satellites_gv_delete();
  satellites_geom_needs_updating = FALSE;
  satellites_on_flag = FALSE;

  return EMPTY_str;
}


/*
 * routine to display fancy satellites
 */
char *
fancy_sat_on_cmd(int argc, char *argv[])
{
  if (use_fancy_satellite)
    return EMPTY_str;

  use_fast_marker_satellite = FALSE;
  use_box_satellite = FALSE;
  use_planes_satellite = FALSE;
  use_sphere_satellite = FALSE;
  use_fancy_satellite = TRUE;

  if (satellites_on_flag) {
    transforms_needed |= (1 << SATELLITES);
    satellites_geom_needs_updating = TRUE;
    gv_start();
    gv_trans_list_create(get_constellation()->satellites);
    write_satellites_geom(get_constellation());
    gv_stop();
  }

  return EMPTY_str;
}

/*
 * routine to display fast marker satellites
 */
char *
fast_marker_sat_on_cmd(int argc, char *argv[])
{
  if (use_fast_marker_satellite)
    return EMPTY_str;

  use_fast_marker_satellite = TRUE;
  use_box_satellite = FALSE;
  use_fancy_satellite = FALSE;
  use_planes_satellite = FALSE;
  use_sphere_satellite = FALSE;

  if (satellites_on_flag) {
    transforms_needed |= (1 << SATELLITES);
    satellites_geom_needs_updating = TRUE;
    gv_start();
    gv_trans_list_create(get_constellation()->satellites);
    write_satellites_geom(get_constellation());
    gv_stop();
  }

  return EMPTY_str;
}

/*
 * routine to display spherical satellites
 */
char *
sphere_sat_on_cmd(int argc, char *argv[])
{
  if (use_sphere_satellite)
    return EMPTY_str;

  use_fast_marker_satellite = FALSE;
  use_box_satellite = FALSE;
  use_fancy_satellite = FALSE;
  use_planes_satellite = FALSE;
  use_sphere_satellite = TRUE;

  if (satellites_on_flag) {
    transforms_needed |= (1 << SATELLITES);
    satellites_geom_needs_updating = TRUE;
    gv_start();
    gv_trans_list_create(get_constellation()->satellites);
    write_satellites_geom(get_constellation());
    gv_stop();
  }

  return EMPTY_str;
}

/*
 * routine to display crossing-planes satellites
 */
char *
planes_sat_on_cmd(int argc, char *argv[])
{
  if (use_planes_satellite)
    return EMPTY_str;

  use_fast_marker_satellite = FALSE;
  use_box_satellite = FALSE;
  use_fancy_satellite = FALSE;
  use_sphere_satellite = FALSE;
  use_planes_satellite = TRUE;

  if (satellites_on_flag) {
    transforms_needed |= (1 << SATELLITES);
    satellites_geom_needs_updating = TRUE;
    gv_start();
    gv_trans_list_create(get_constellation()->satellites);
    write_satellites_geom(get_constellation());
    gv_stop();
  }

  return EMPTY_str;
}


/*
 * routine to display box satellites
 */
char *
box_sat_on_cmd(int argc, char *argv[])
{
  if (use_box_satellite)
    return EMPTY_str;

  use_fast_marker_satellite = FALSE;
  use_fancy_satellite = FALSE;
  use_planes_satellite = FALSE;
  use_sphere_satellite = FALSE;
  use_box_satellite = TRUE;

  if (satellites_on_flag) {
    transforms_needed |= (1 << SATELLITES);
    satellites_geom_needs_updating = TRUE;
    gv_start();
    gv_trans_list_create(get_constellation()->satellites);
    write_satellites_geom(get_constellation());
    gv_stop();
  }

  return EMPTY_str;
}


/*
 * Routine to turn off display of fast marker satellites
 */
char *
fast_marker_sat_off_cmd(int argc, char *argv[])
{
  if (!use_fast_marker_satellite)
    return EMPTY_str;

  use_sphere_satellite = TRUE;
  use_fast_marker_satellite = FALSE;

  if (satellites_on_flag) {
    transforms_needed &= ~(1 << SATELLITES);
    satellites_geom_needs_updating = TRUE;
    gv_start();
    write_satellites_geom(get_constellation());
    gv_stop();
  }

  return EMPTY_str;
}


/*
 * Routine to turn off display of spherical satellites
 */
char *
sphere_sat_off_cmd(int argc, char *argv[])
{
  if (!use_sphere_satellite)
    return EMPTY_str;

  use_sphere_satellite = FALSE;
  use_fast_marker_satellite = TRUE;

  if (satellites_on_flag) {
    transforms_needed &= ~(1 << SATELLITES);
    satellites_geom_needs_updating = TRUE;
    gv_start();
    write_satellites_geom(get_constellation());
    gv_stop();
  }

  return EMPTY_str;
}


/*
 * Routine to turn off display of fancy satellites
 */
char *
fancy_sat_off_cmd(int argc, char *argv[])
{
  if (!use_fancy_satellite)
    return EMPTY_str;

  use_fancy_satellite = FALSE;
  use_fast_marker_satellite = TRUE;

  if (satellites_on_flag) {
    transforms_needed &= ~(1 << SATELLITES);
    satellites_geom_needs_updating = TRUE;
    gv_start();
    write_satellites_geom(get_constellation());
    gv_stop();
  }

  return EMPTY_str;
}


/*
 * Routine to turn off display of crossing-plane satellites
 */
char *
planes_sat_off_cmd(int argc, char *argv[])
{
  if (!use_planes_satellite)
    return EMPTY_str;

  use_planes_satellite = FALSE;
  use_fast_marker_satellite = TRUE;

  if (satellites_on_flag) {
    transforms_needed &= ~(1 << SATELLITES);
    satellites_geom_needs_updating = TRUE;
    gv_start();
    write_satellites_geom(get_constellation());
    gv_stop();
  }

  return EMPTY_str;
}


/*
 * Routine to turn off display of box satellites
 */
char *
box_sat_off_cmd(int argc, char *argv[])
{
  if (!use_box_satellite)
    return EMPTY_str;

  use_box_satellite = FALSE;
  use_fast_marker_satellite = TRUE;

  if (satellites_on_flag) {
    transforms_needed &= ~(1 << SATELLITES);
    satellites_geom_needs_updating = TRUE;
    gv_start();
    write_satellites_geom(get_constellation());
    gv_stop();
  }

  return EMPTY_str;
}

char *
satellites_set_cmd(int argc, char *argv[])
{
  Constellation *pconstellation = get_constellation();
  Satellite s;

  int n = atoi(argv[2]);
  unsigned int flag = atoi(argv[3]);

  s = constellation_search(pconstellation, n);
  if (s) {
    if (flag)
      s->can_display_satellite = 1;
    else
      s->can_display_satellite = 0;
    satellites_geom_needs_updating = TRUE;
  }

  return EMPTY_str;
}


/*
 * satellites_gv_delete
 *
 * routine to delete geomview satellites object
 */
void
satellites_gv_delete()
{
  if (satellites_geom_exists) {
    gv_start();
    gv_delete_geom("Satellites");
    gv_stop();
    satellites_geom_exists = FALSE;
  }
}

/*
 * satellite_display
 *
 * Takes a satellite and displays it
 */
void
satellite_display(Satellite s, const Constellation * pconstellation)
{
  satellites_geom_needs_updating = TRUE;
}

/*
 * satellites_relocate
 *
 * Moves a satellite into its proper position
 */
void
satellites_relocate(const Constellation * pconstellation)
{
  /* fancy satellites automatically updated from transformation matrices */
  if (use_fancy_satellite)
    return;

  satellites_geom_needs_updating = TRUE;
  gv_start();
  write_satellites_geom(pconstellation);
  gv_stop();
}

/*
 * satellite_delete
 *
 * Takes a satellite and deletes it on the display
 */
void
satellite_delete(Satellite s)
{
  satellites_geom_needs_updating = TRUE;
}


void
write_satellites_geom(const Constellation * pconstellation)
{
  Satellite s;
  Satellite_list sl;
  unsigned int i, num_view_sats, num_tag_sats;

  num_view_sats = pconstellation->n_view_sats;
  num_tag_sats = pconstellation->n_tag_sats;

  if (satellites_on_flag && satellites_geom_needs_updating) {

    satellites_geom_needs_updating = FALSE;

    /* if no sats then delete the Satellites gv object */
    if ((num_view_sats == 0) &&
	(num_tag_sats == 0)) {
      satellites_gv_delete();
      return;
    }

    if (!use_fast_marker_satellite) {

      /* write out new list of all satellites */
      sl = pconstellation->satellites;
      gv_send("(geometry Satellites {LIST\n");
      while (sl) {
	s = sl->s;
	if (sl->s->can_display_satellite) {
	  if (use_box_satellite) {
	    fprintf(gv_out, "{INST transform:trans_%d geom:%s}\n", s->id,
		    s->tag ? box_special_hname : box_hname);
	  } else if (use_planes_satellite) {
	    fprintf(gv_out, "{INST transform:trans_%d geom:%s}\n", s->id,
		    s->tag ? planes_special_hname : planes_hname);
	  } else if (use_sphere_satellite) {
	    fprintf(gv_out, "{INST transform:trans_%d geom:%s}\n", s->id,
		    s->tag ? sphere_special_hname : sphere_hname);
	  } else {
	    /* use fancy satellite */
	    fprintf(gv_out, "{INST transform:trans_%d geom:%s}\n", s->id,
		    s->tag ? fancy_special_hname : fancy_hname);
	  }
	}
	sl = sl->next;
      }
      gv_send("})\n");

    } else {

      /* write out as a LIST of VECT */
      gv_send("(geometry Satellites {LIST\n");

      /* write out all viewable satellites as a single VECT */
      if (num_view_sats > 0) {
       if (GV_BINARY_FORMAT_AVAILABLE) {
	uint32_t ibuf[3];
	float fbuf[3];
	float rgba[] = { 0, 1, 0, 1 };	/* green */
	uint16_t zero_one_buf[] = { 0, 1 };
	uint16_t *zero = zero_one_buf;
	uint16_t *one = zero_one_buf + 1;

	/* convert stuff to big endian format */
	htons_buffer(zero, 2);

	/* send VECT header */
	gv_send("{appearance {linewidth 7} VECT BINARY\n");
	/* send: n_view_sats n_view_sats 1 */
	ibuf[0] = ibuf[1] = num_view_sats;
	ibuf[2] = 1;
	htonl_buffer(ibuf, 3);
	gv_send_binary_ints(ibuf, 3);
	/* send n_view_sats number of short 1s */
	for (i = 0; i < num_view_sats; i++) {
	  gv_send_binary_shorts(one, 1);
	}
	/* send short 1 followed by n_view_sats-1 number of short 0s */
	gv_send_binary_shorts(one, 1);
	for (i = 1; i < num_view_sats; i++) {
	  gv_send_binary_shorts(zero, 1);
	}
	/* for each satellite to view, send 3 floats describing position */
	for (sl = pconstellation->satellites; NULL != sl; sl = sl->next) {
	  s = sl->s;
	  if (s->can_display_satellite) {
	    fbuf[0] = s->transform[3][0];
	    fbuf[1] = s->transform[3][1];
	    fbuf[2] = s->transform[3][2];
	    htonl_buffer((uint32_t *) fbuf, 3);
	    gv_send_binary_floats(fbuf, 3);
	  }
	}
	/* send color */
	htonl_buffer((uint32_t *) rgba, 4);
	gv_send_binary_floats(rgba, 4);
	gv_send("}\n");
       } else {
	gv_send("{appearance {linewidth 7} VECT\n");
	fprintf(gv_out, "%d %d 1\n", num_view_sats, num_view_sats);
	for (i = 0; i < num_view_sats; i++) {
	  gv_send("1 ");
	}
	gv_send("\n1 ");
	for (i = 1; i < num_view_sats; i++) {
	  gv_send("0 ");
	}
	gv_send("\n");
	for (sl = pconstellation->satellites; NULL != sl; sl = sl->next) {
	  s = sl->s;
	  if (s->can_display_satellite) {
	    fprintf(gv_out, "%f %f %f\n", s->transform[3][0],
		    s->transform[3][1], s->transform[3][2]);
	  }
	}
	gv_send("0 1 0 1}\n");
       }
      }

      /* write out any tagged satellites */
      if (num_tag_sats > 0) {
	fprintf(gv_out, "{appearance {linewidth 15} VECT\n%d %d 1\n",
		num_tag_sats, num_tag_sats);
	for (i = 0; i < num_tag_sats; i++) {
	  gv_send("1 ");
	}
	gv_send("\n1 ");
	for (i = 1; i < num_tag_sats; i++) {
	  gv_send("0 ");
	}
	gv_send("\n");
	for (sl = pconstellation->satellites; NULL != sl; sl = sl->next) {
	  s = sl->s;
	  if (s->tag) {
	    fprintf(gv_out, "%f %f %f\n", s->transform[3][0],
		    s->transform[3][1], s->transform[3][2]);
	  }
	}
	gv_send("1 1 0 1}\n");
      }

      /* close LIST object */
      gv_send("})\n");
    }

    satellites_geom_exists = TRUE;
  }

}
