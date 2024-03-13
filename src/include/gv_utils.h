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
 * gv_utils.h
 *
 * $Id: gv_utils.h,v 1.22 2017/01/02 06:11:32 lloydwood Exp $
 */

#ifndef _GV_UTILS_H_
#define _GV_UTILS_H_

#include <limits.h>

#include "int_types.h"

/*
 * Try to check if machine has 32-bit unsigned ints and 32-bit floats
 * and 16-bit shorts, then use a more efficient method of writing to
 * Geomview. The test is conservative; you may increase performance
 * with use of binary formats by removing the check, at your own risk.
 */
#if ((INT_MAX == 2147483647) && (USHRT_MAX == 65535))
#define TEST_FOR_GV_BINARY_FORMAT
#endif

unsigned char GV_BINARY_FORMAT_AVAILABLE;

unsigned char test_ieee_float(void);

unsigned char gv_stream_init_proc(void);
unsigned char gv_init_proc(void);
void gv_send(const char *);
char *gv_version(void);
void gv_send_binary_ints(const uint32_t *buf, unsigned int n);
void gv_send_binary_shorts(const uint16_t *buf, unsigned int n);
void gv_send_binary_floats(const float *buf, unsigned int n);
unsigned int gv_sendfile(const char *);
void gv_wait(void);
void gv_begin(void);
void gv_start(void);
void gv_end(void);
void gv_stop(void);
void gv_transform(const char *, double [4][4]);
void gv_create_geom(const char *, const char *, const char *);
void gv_create_geomh(const char *, const char *);
void gv_create_alienh(const char *, const char *);
void gv_delete_geom(const char *);
void gv_delete_handle(const char *);
void gv_ui_freeze(int flag);
void gv_set_ready(void);
unsigned int gv_ready(void);

char *gv_begin_cmd(int argc, char *argv[]);
char *gv_end_cmd(int argc, char *argv[]);
char *gv_wait_cmd(int argc, char *argv[]);
char *gv_send_cmd(int argc, char *argv[]);

void htonl_buffer(uint32_t *buf, unsigned int n);
void htons_buffer(uint16_t *buf, unsigned int n);

#endif
/* !_GV_UTILS_H_ */
