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
 * gv_utils.c
 *
 * $Id: gv_utils.c,v 1.36 2017/01/02 06:11:32 lloydwood Exp $
 */
#include <stdio.h>
#include <math.h>
#include <string.h>
/* #include <bstring.h> */
/* for select */
#include <unistd.h>
#include <sys/time.h>

#include <netinet/in.h>

#include <sys/select.h>
/* extern int select(int, int *, int *, int *, struct timeval *); */

#include "int_types.h"
#include "utils.h"
#include "gv_file.h"
#include "gv_utils.h"
#include "constants.h"
#include "globals.h"

extern void gv_delayed_view_update(void);

static unsigned char GV_LITTLE_ENDIAN = 1;
static unsigned char GV_BIG_ENDIAN = 2;

/*
 * variable to see if an acknowledgement request has been sent
 *  accessed by gv_set_ready and gv_ready
 */
static unsigned int gv_ack = FALSE;

/*
 * if depth nesting ever goes negative, it's a bug.
 */
static int progn_depth = 0;

/*
 * get information on floating point storage type.
 * Suggested by Stuart Levy on geomview-users, November 2000
 */
unsigned char
test_ieee_float()
{
   static float testfloat = 40970.25;
   static unsigned char testle[] = { 0x40, 0x0a, 0x20, 0x47 };
   static unsigned char testbe[] = { 0x47, 0x20, 0x0a, 0x40 };

   if(sizeof(testfloat) != 4)
        return 0;                       /* wrong float size */
   if(memcmp( &testfloat, testle, 4 ) == 0)
        return GV_LITTLE_ENDIAN;                       /* OK, little-endian */
   if(memcmp( &testfloat, testbe, 4 ) == 0)
        return GV_BIG_ENDIAN;                       /* OK, big-endian */

   return 0;                            /* non-IEEE, or something */
}


unsigned char
gv_stream_init_proc()
{
  /*
   * Redirect stdout to stderr
   */
  int gv_fdout, fdout;

  gv_in = stdin;

  fdout = fileno(stdout);
  gv_fdout = dup(fdout);
  if (gv_fdout == NOT_OK) {
    error("failure to set up output pipe to control Geomview.");
    return FALSE;
  }
  close(fdout);

  gv_out = fdopen(gv_fdout, "a");
  if (!gv_out) {
    error("failure to open output pipe to control Geomview.");
    return FALSE;
  }

  if (dup2(fileno(stderr), fdout) != fdout) {
    error("failure to redirect stdout to stderr.");
    fclose(gv_out);
    return FALSE;
  }
  /* at this point stdout should point to stderr */

  return TRUE;
}

/*
 * gv_init_proc
 *
 * Call to initialize any variables, if necessary.
 *
 */
unsigned char
gv_init_proc()
{
  unsigned char gv_binary_format_available;

  gv_binary_format_available = test_ieee_float();

#ifndef TEST_FOR_GV_BINARY_FORMAT
  error("can't use Geomview binary format on this architecture.");
  gv_binary_format_available = 0;
#endif

  return gv_binary_format_available;
}

/*
 * gv_send
 *
 * Sends a string to geomview
 */
void
gv_send(const char *buf)
{
  if (!geomview_module)
    return;
  fprintf(gv_out, "%s", buf);
}

/*
 * gv_version
 *
 * Returns a pointer to a string containing the version of geomview
 */
char *
gv_version()
{
  static char version_str[LENGTH_STRING_BUFFER];
  unsigned int c;
  char *p;

  /* request version from geomview */
  gv_wait();
  gv_send("(echo (geomview-version))\n");
  fflush(gv_out);

  /* get a double quote */
  do {
    c = fgetc(gv_in);
  } while (c != '"');

  /* read in characters until another double quote is found */
  p = version_str;
  do {
    *p = (char) fgetc(gv_in);
  } while ((*(p++)) != '"');

  /* overwrite final double quote with a string terminator */
  *(--p) = '\0';

  return version_str;
}

/*
 * gv_send_binary_ints
 *
 * Sends long ints in (big endian) binary format to geomview
 * To convert to big endian format, use htonl_buffer
 *
 */
void
gv_send_binary_ints(const uint32_t *buf, unsigned int n)
{
  fwrite((const void *) buf, 4, n, gv_out);
}

/*
 * gv_send_binary_shorts
 *
 * Sends short ints in (big endian) binary format to geomview
 * To convert to big endian format, use htons_buffer
 *
 */
void
gv_send_binary_shorts(const uint16_t *buf, unsigned int n)
{
  fwrite((const void *) buf, 2, n, gv_out);
}

/*
 * gv_send_binary_floats
 *
 * Sends floats in (big endian) binary format to geomview
 * To convert to big endian format, use htonl_buffer
 * Presumes that a float is a 32-bit (4-byte) word.
 *
 */
void
gv_send_binary_floats(const float *buf, unsigned int n)
{
  fwrite((const void *) buf, 4, n, gv_out);
}

/*
 * gv_sendfile
 *
 * Sends a file to geomview
 */
unsigned int
gv_sendfile(const char *filename)
{
  return copyfile(gv_out, filename);
}

/*
 * gv_wait
 *
 * Waits until geomview is up to date
 */
void
gv_wait()
{
  while (!gv_ready());	/* just in case something is waiting already */
  gv_set_ready();
  while (!gv_ready());
}

/*
 * gv_begin
 *
 * Sets up beginning of block of commands.
 *
 */
void
gv_begin()
{
  if (!geomview_module) {
    error("gv_begin() called although not running as Geomview module.");
    return;
  }

  if (0 == progn_depth++) {
    fprintf(gv_out, "(progn\n");
  }
}

void
gv_start()
{
  if (!geomview_module) {
    error("gv_start() called although not running as Geomview module.");
    return;
  }

  if (0 == progn_depth++) {
    fprintf(gv_out, "(progn\n");
  }
}

/*
 * gv_end
 *
 * Ends block of commands and flushes buffer
 */
void
gv_end()
{

  if (!geomview_module) {
    error("gv_end() called although not running as Geomview module.");
    return;
  }

  if (--progn_depth == 0) {
    gv_delayed_view_update();
    fprintf(gv_out, ")\n");
    fflush(gv_out);
  } else if (progn_depth < 0) {
    error("gv_end() called more than gv_begin()! Nesting symmetry broken.");
  }
}

/*
 * gv_stop
 * As gv_end(), but doesn't call gv_delayed_view_update() to update everything.
 */
void
gv_stop()
{

  if (!geomview_module) {
    error("gv_stop() called although not running as Geomview module.");
    return;
  }

  if (--progn_depth == 0) {
    fprintf(gv_out, ")\n");
    fflush(gv_out);
  } else if (progn_depth < 0) {
    error("gv_stop() called too often! Nesting symmetry broken.");
  }
}


/*
 * gv_transform
 *
 * Defines transform name using matrix m.
 */
void
gv_transform(const char *name, double m[4][4])
{
  unsigned int i, j;

  fprintf(gv_out, "(read transform {transform define %s\n", name);
  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++)
      fprintf(gv_out, "%g ", m[i][j]);
  fprintf(gv_out, "})\n");
}

/*
 * gv_create_geom
 *
 * Create geomview object name as the transformation tname
 * applied to the object hname.
 */
void
gv_create_geom(const char *name, const char *tname, const char *hname)
{
  fprintf(gv_out, "(geometry %s { INST transform :%s geom :%s} )\n",
	  name, tname, hname);
}

/*
 * gv_create_geomh
 *
 * Create geomview object name from handle hname.
 */
void
gv_create_geomh(const char *name, const char *hname)
{
  fprintf(gv_out, "(geometry %s { :%s } )\n", name, hname);
}

/*
 * gv_create_alienh
 *
 * Create geomview object name from handle hname.
 */
void
gv_create_alienh(const char *name, const char *hname)
{
  fprintf(gv_out, "(new-alien %s { :%s } )\n", name, hname);
}



/*
 * gv_delete_geom(char *name)
 *
 * Deletes geomview object name
 */
void
gv_delete_geom(const char *name)
{
  fprintf(gv_out, "(delete %s)\n", name);
}

/*
 * gv_delete_handle(char *name)
 *
 * Deletes geomview handle object
 */
void
gv_delete_handle(const char *name)
{
  fprintf(gv_out, "(read geometry {define %s {} })\n", name);
}


/*
 * gv_ui_freeze(int flag)
 *
 * If flag is TRUE then geomview interface is frozen
 * else if FALSE it is reactivated. Currently unused.
 */
void
gv_ui_freeze(int flag)
{
  if (!flag) {
    fprintf(gv_out, "(ui-freeze off)\n");
    fflush(gv_out);
  } else {
    fprintf(gv_out, "(ui-freeze on)\n");
  }
}

/*
 * send msg to gv so that we can later test if it is ready
 */
void
gv_set_ready()
{
  if (gv_ack)
    return;
  fprintf(gv_out, "(echo \"\\n\")\n");
  fflush(gv_out);
  gv_ack = TRUE;
}

/*
 * returns true if GV has returned the last acknowledgement
 * sent by gv_set_ready
 */
unsigned int
gv_ready()
{
  char c;
  static struct timeval poll = { 0, 100000 };
  fd_set fds;

  /* don't wait for input from Geomview if it's not really there. */
  if (fake_geomview_module) return TRUE;

  FD_ZERO(&fds);
  FD_SET(fileno(gv_in), &fds);

  /* if nothing to wait for then ready */
  if (!gv_ack)
    gv_set_ready();

  if (select(fileno(gv_in) + 1, &fds, NULL, NULL, &poll) > 0) {
    c = (char)fgetc(gv_in);
    if (c != '\n')
      fprintf(stderr, "gv_ready: error, got char: %c (%d)\n", c, c);
    gv_ack = FALSE;

    if (debug) {
      putc('|', stderr);
      putc('\n', stderr);
    }

    return TRUE;
  }

  if (debug) putc('.', stderr);

  return FALSE;
}


char *
gv_begin_cmd(int argc, char *argv[])
{
  if (geomview_module) {
    gv_begin();
  }
  return EMPTY_str;
}

char *
gv_end_cmd(int argc, char *argv[])
{
  if (geomview_module) {
    gv_end();
  }
  return EMPTY_str;
}

char *
gv_wait_cmd(int argc, char *argv[])
{
  gv_wait();
  return EMPTY_str;
}

char *
gv_send_cmd(int argc, char *argv[])
{
  if (!geomview_module)
    return EMPTY_str;
  if (argc == 3) {
    gv_begin();
    gv_send(argv[2]);
    gv_end();
  } else {
    error_format("gv_send_cmd: savi %s needs 1 argument", argv[1]);
  }
  return EMPTY_str;
}

/*
 * htonl_buffer
 *
 * Converts an array of unsigned ints from host format
 * to native (big endian) format.
 * This is for writing in binary format to Geomview and
 * requires use of an unsigned 32-bit word.
 */
void
htonl_buffer(uint32_t *buf, unsigned int n)
{
  /*
   * if we are not little-endian, do nothing.
   */
  if (GV_BINARY_FORMAT_AVAILABLE != GV_LITTLE_ENDIAN) return;

  while ((n--) > 0) {
    *buf = htonl(*buf);
    buf++;
  }
}

/*
 * htons_buffer
 *
 * Converts an array of unsigned shorts from host format
 * to native (big endian) format.
 * This is for writing in binary format to Geomview and
 * requires use of an unsigned 16-bit (two-byte) word.
 */
void
htons_buffer(uint16_t *buf, unsigned int n)
{
  /*
   * if we are not little-endian, do nothing.
   */
  if (GV_BINARY_FORMAT_AVAILABLE != GV_LITTLE_ENDIAN) return;

  while ((n--) > 0) {
    *buf = htons(*buf);
    buf++;
  }
}
