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
 * savi.h
 *
 * headers for the different functions
 *
 * $Id: savi.h,v 1.33 2017/01/02 06:11:32 lloydwood Exp $
 */

#ifndef _SAVI_H_
#define _SAVI_H_

#include "Satellite.h"
#include "stats_utils.h"

/* gv_init.c */
void gv_sat_create(Satellite, const Constellation *pconstellation);
void gv_trans_create(Satellite);
void gv_sat_list_create(const Constellation *pconstellation);
void gv_trans_list_create(Satellite_list);
unsigned int gv_init(void);

/* earth.c */
void redraw_earth(int argc, char *argv[]);
char *earth_off_cmd(int argc, char *argv[]);
void earth_place(double t, const CentralBody *pcb);
char *earth_on_cmd(int argc, char *argv[]);
char *simple_off_cmd(int argc, char *argv[]);
char *simple_on_cmd(int argc, char *argv[]);
char *fancy_off_cmd(int argc, char *argv[]);
char *fancy_on_cmd(int argc, char *argv[]);
char *texture_on_cmd(int argc, char *argv[]);
char *texture_off_cmd(int argc, char *argv[]);
char *geomview_dynamic_texture_on_cmd(int argc, char *argv[]);
char *geomview_dynamic_texture_off_cmd(int argc, char *argv[]);

/* sun.c */
char *sun_off_cmd(int argc, char *argv[]);
void sun_place(double t, const CentralBody *pcb);
char *sun_on_cmd(int argc, char *argv[]);

/* plane.c */
char *plane_cmd(int argc, char *argv[]);
char *plane_off_cmd(int argc, char *argv[]);

/* satellites.c */
char *satellites_on_cmd(int argc, char *argv[]);
char *satellites_off_cmd(int argc, char *argv[]);
char *satellites_set_cmd(int argc, char *argv[]);
char *fancy_sat_on_cmd(int argc, char *argv[]);
char *fancy_sat_off_cmd(int argc, char *argv[]);
char *planes_sat_on_cmd(int argc, char *argv[]);
char *planes_sat_off_cmd(int argc, char *argv[]);
char *box_sat_on_cmd(int argc, char *argv[]);
char *box_sat_off_cmd(int argc, char *argv[]);
char *sphere_sat_on_cmd(int argc, char *argv[]);
char *sphere_sat_off_cmd(int argc, char *argv[]);
char *fast_marker_sat_on_cmd(int argc, char *argv[]);
char *fast_marker_sat_off_cmd(int argc, char *argv[]);

void satellite_display(Satellite s, const Constellation *pconstellation);
void satellites_relocate(const Constellation *pconstellation);
void satellite_delete(Satellite s);
void satellites_gv_delete(void);
void write_satellites_geom(const Constellation *pconstellation);

/* orbits.c */
char *orbits_on_cmd(int argc, char *argv[]);
char *orbits_off_cmd(int argc, char *argv[]);
char *orbits_set_cmd(int argc, char *argv[]);
void orbit_display(Satellite s, const Constellation *pconstellation);
void orbits_relocate(const Constellation *pconstellation);
void orbit_delete(Satellite s);
void orbits_gv_delete(void);
void write_orbits_geom(const Constellation *pconstellation);

/* cones.c */
char *cones_on_cmd(int argc, char *argv[]);
char *cones_off_cmd(int argc, char *argv[]);
void cones_rebuild(void);
void cones_relocate(const Constellation *pconstellation);
void cone_display(Satellite, const Constellation *pconstellation);
void cone_delete(Satellite s);
void cones_gv_update(void);
void cones_gv_delete(void);
void write_cones_geom(const Constellation *pconstellation);

/* footprints.c */
char *footprints_on_cmd(int argc, char *argv[]);
char *footprints_off_cmd(int argc, char *argv[]);
void footprints_rebuild(void);
void footprints_relocate(const Constellation *pconstellation);
void footprint_display(Satellite s, const Constellation *pconstellation);
void footprint_delete(Satellite s);
void footprints_gv_update(void);
void footprints_gv_delete(void);
void write_footprints_geom(const Constellation *pconstellation);

/* coverage.c */
void redraw_coverage_display(void);
void update_coverage_display(void);
void clear_coverage(void);
void coverage_decay(void);
void coverage_compute(const Satellite_list sl, unsigned int,
		      const CentralBody *pcb);
void coverage_display(const Satellite_list sl, unsigned int,
		      const CentralBody *pcb);
char *coverage_on_cmd(int argc, char *argv[]);
char *coverage_off_cmd(int argc, char *argv[]);
char *coverage_set_cmd(int argc, char *argv[]);
char *no_access_on_cmd(int argc, char *argv[]);
char *no_access_off_cmd(int argc, char *argv[]);
char *print_access_on_cmd(int argc, char *argv[]);
char *print_access_off_cmd(int argc, char *argv[]);
char *map_on_cmd(int argc, char *argv[]);
char *map_off_cmd(int argc, char *argv[]);
int get_map_flag(void);
int get_coverage_type(void);
void tracks_compute(const Satellite_list sl, const CentralBody *pcb);
char *tracks_on_cmd(int argc, char *argv[]);
char *tracks_off_cmd(int argc, char *argv[]);
char *project_tracks_on_cmd(int argc, char *argv[]);
char *project_tracks_off_cmd(int argc, char *argv[]);
void project_tracks_reset(void);
char *coverage_proj_cmd(int argc, char *argv[]);
char *coverage_save_map_cmd(int argc, char *argv[]);
char *coverage_angle_type_cmd(int argc, char *argv[]);
char *coverage_color_update_cmd(int argc, char *argv[]);
double mean_coverage(int projection_type, grid *g);

void init_coverage_time(void);
void update_coverage_time(void);
double total_coverage(int projection_type, int output, grid *g);

/* axes.c */
char *axes_on_cmd(int argc, char *argv[]);
char *axes_off_cmd(int argc, char *argv[]);

/* logo.c */
char *logo_on_cmd(int argc, char *argv[]);
char *logo_off_cmd(int argc, char *argv[]);

/* stars.c */
char *stars_on_cmd(int argc, char *argv[]);
char *stars_off_cmd(int argc, char *argv[]);

typedef struct ViewModule {
  /* function called to extend geom to include new satellite
     or rewrite old satellite stuff; called by delayed_stuff */
  void (*write_geom_fn)(const Constellation *);

  /* function that must be called if the coverage angle/type is changed;
     called by coverage_angle_type_cmd */
  void (*coverage_rebuild_fn)(void);

  /* function used to display object when a
     new satellite is created; called by satellite_gv_create */
  void (*display_fn)(Satellite, const Constellation *);

  /* function used to relocate objects when the satellites have movied;
     called by satellite_gv_create */
  void (*relocate_fn)(const Constellation *);

  /* function to delete item associated to an individual satellite;
     called by sats_delete_cmd */
  void (*delete_fn)(Satellite);

  /* function called to turn options off; called by exit_cmd.
     These functions usually also call the corresponding gv_delete_fn. */
  char *(*off_cmd)(int, char *[]);

  /* function called to eliminate all trace of object in Geomview;
     called by sats_delete_all when all the satellites are deleted. */
  void (*gv_delete_fn)(void);

} ViewModule;

typedef struct CompModule {
  void (*update_fn)(const Constellation *);
} CompModule;

extern ViewModule view_modules[];
const int N_VIEW_MODULES;
extern CompModule comp_modules[];
const int N_COMP_MODULES;

/*
 * These are used to provide unique identifiers for modules which
 * require certain information to be computed.  They are used like:
 *
 *          transforms_needed |= (1 << FOOTPRINTS);     <--- on
 *          transforms_needed &= ~(1 << FOOTPRINTS);    <--- off
 */
typedef enum {
  SATELLITES=0, CONES, FOOTPRINTS
} Module_Codes;

#endif
/* !_SAVI_H_ */
