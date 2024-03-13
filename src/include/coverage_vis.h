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
 * coverage_vis.h
 *
 * $Id: coverage_vis.h,v 1.39 2017/01/02 06:11:32 lloydwood Exp $
 */

#ifndef _COVERAGE_VIS_H_
#define _COVERAGE_VIS_H_

#include "stats_utils.h"
#include "Satellite.h"

/* bitmap of geographical boundaries for coverage */
#define CYLINDRICAL_BITMAP_NAME "maps/world.cyl.pbm"
#define UNPROJECTED_BITMAP_NAME "maps/world.unp.pbm"
#define UNPROJECTED_MASK_BITMAP_NAME "maps/world-mask.unp.pbm"
#define SPHERICAL_BITMAP_NAME "maps/world.sph.pbm"
#define SPHERICAL_90_BITMAP_NAME "maps/world90W.sph.pbm"
#define SINUSOIDAL_BITMAP_NAME "maps/world.sinu.pbm"
#define SINUSOIDAL_90_BITMAP_NAME "maps/world90W.sinu.pbm"

#define CYLINDRICAL_LARGE_BITMAP_NAME "maps/world1024.cyl.pbm"
#define UNPROJECTED_LARGE_BITMAP_NAME "maps/world1024.unp.pbm"
#define UNPROJECTED_MASK_LARGE_BITMAP_NAME "maps/world-mask1024.unp.pbm"
#define SPHERICAL_LARGE_BITMAP_NAME "maps/world1024.sph.pbm"
#define SPHERICAL_90_LARGE_BITMAP_NAME "maps/world90W1024.sph.pbm"
#define SINUSOIDAL_LARGE_BITMAP_NAME "maps/world1024.sinu.pbm"
#define SINUSOIDAL_90_LARGE_BITMAP_NAME "maps/world90W1024.sinu.pbm"

#define LONGITUDE_CENTER_LINE 0.0
#define LONGITUDE_CENTER_LINE_90W (-90.0)

/*
 * Indices to be inserted in the foreground array drawn over coverage.
 */
enum {
    COVERAGE_AREA_CI = 0,
    MAP_OUTLINE_CI,
    PROJECT_TRACKS_CI,
    SPECIAL_PROJECT_TRACKS_CI,
    GROUND_TRACKS_CI,
    GROUND_TRACKS_CROSS_CI,
    SPECIAL_GROUND_TRACKS_CI,
    SPECIAL_GROUND_TRACKS_CROSS_CI,
    GROUND_LOCATION_CI,
    GROUND_LOCATION_CROSS_CI
};

void write_image_to_file(char *name);

int coverage_dynamic_initialised(void);

int capable_of_compression(void);

void coverage_color_copy(void);
int coverage_custom_height_valid(int map_view_height);
void image_init(grid *g);
void clear_interval(grid *g);
void update_display(int projection, grid *g, int coverage_flag,
			   int tracks_flag, int project_tracks_flag);
void coverage_send_static_file(void);
void coverage_send_scratchfile(void);
void coverage_dynamic_cleanup(void);
void reset_foreground(int projection, grid *g);
void tracks_to_foreground(const Satellite_list SL, int projection, grid *g,
			  unsigned int color_index, const CentralBody *pcb);
void latlon_to_foreground(LatLon *pl, int projection, grid * g, unsigned int color_index);
void fisheye_to_foreground(int projection, grid * g);
void track_to_foreground(Satellite s, int projection, grid *g,
			 unsigned int color_index, const CentralBody *pcb);
void grid_and_foreground_to_image(int projection, grid * g, int fgOnly);

#endif
/* !_COVERAGE_VIS_H */
