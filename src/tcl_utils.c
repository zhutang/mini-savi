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
 * tcl_utils.c
 *
 * $Id: tcl_utils.c,v 1.45 2017/06/26 04:33:07 lloydwood Exp $
 */

#include "constants.h"
#include "sats.h"
#include "utils.h"
#include "globals.h"
#include "tcl_utils.h"

static Tk_Window mainWindow;	/* The main window for the application. */

/* we want to bypass Tcl_Eval for speed in some case, so store some cmd info */
static Tcl_CmdInfo update_info;

static unsigned int tcl_app_init(void);
static unsigned int prepend_to_auto_path(const char []);

/*
 * tcl_init
 *
 * Initialisation of tcl interface
 */
unsigned int
tcl_init(Tcl_Interp * interpreter)
{
  if (Tcl_Init(interpreter) == TCL_ERROR) {
#if (TK_MAJOR_VERSION >= 8)
    error_format("tcl_init: Error in Tcl_Init: %s", Tcl_GetStringResult(interpreter));
#else
    error_format("tcl_init: Error in Tcl_Init: %s", interpreter->result);
#endif
    return FALSE;
  }
  return TRUE;
}

/*
 * tk_init
 *
 * Initialization of tk interface
 */
unsigned int
tk_init(Tcl_Interp * interpreter)
{
  void * screenName = NULL;
  static char baseName[] = "Satellites";
  static char className[] = "";

  if (Tk_Init(interpreter) == TCL_ERROR) {
#if (TK_MAJOR_VERSION >= 8)
    error_format("tk_init: Error in Tk_Init: %s", Tcl_GetStringResult(interpreter));
#else
    error_format("tk_init: Error in Tk_Init: %s", interpreter->result);
#endif
    return FALSE;
  }
  if (!tcl_app_init()) {
#if (TK_MAJOR_VERSION >= 8)
    error_format("tk_init: Error in tcl_app_init: %s", Tcl_GetStringResult(interpreter));
#else
    error_format("tk_init: Error in tcl_app_init: %s", interpreter->result);
#endif
    return FALSE;
  }

#if ( (TK_MAJOR_VERSION > 4) || ( (TK_MAJOR_VERSION == 4) && (TK_MINOR_VERSION > 0)) )
  if (!(mainWindow = Tk_CreateWindow(interpreter, screenName, baseName, className))) {
#  if (TK_MAJOR_VERSION >= 8)
    error_format("tk_init: Can't create main wainWindow: %s", Tcl_GetStringResult(interpreter));
#  else
    error_format("tk_init: Can't create main wainWindow: %s", interpreter->result);
#  endif
    return FALSE;
  }
#else
  if (!(mainWindow = Tk_CreateMainWindow(interpreter, screenName, baseName, className))) {
#  if (TK_MAJOR_VERSION >= 8)
    error_format("tk_init: Can't create main wainWindow: %s", Tcl_GetStringResult(interpreter));
#  else
    error_format("tk_init: Can't create main wainWindow: %s", interpreter->result);
#  endif
    return FALSE;
  }
#endif

  return (tk_update());
}

/*
 * tcl_app_init
 *
 * Application specific initialization commands for tcl/tk
 *
 */
static unsigned int
tcl_app_init()
{
  CentralBody *pcb = get_constellation()->pcb;
  static char command0[] = "satellites";
  static char command1[] = "savi";
  static char update[] = "update";
  static char var0[] = "delta_t";
  static char var1[] = "coverage_angle";
  static char var2[] = "IM_W";
  static char var3[] = "IM_H";
  static char var4[] = "params(Radius)";
  static char var5[] = "params(Mu)";
  static char var6[] = "params(Omega)";
  static char var7[] = "params(J2)";
  static char var8[] = "params(orbital_period)";
  static char var9[] = "params(tilt)";
  static char var10[] = "tracks_interval";
  static char var11[] = "FS_D";
  static char var12[] = "NUM_COLORS";
  static char var13[] = "coverage_projection";
  static char var14[] = "Longitude_Center_Line";
  static char var15[] = "coverage_display_center_longitude";
  static char var16[] = "first_filename";
  static char var17[] = "debug";
  static char var18[] = "splash_about";
  static char var19[] = "DIV(0)";
  static char var20[] = "DEC(0)";
  static char var21[] = "DIV(1)";
  static char var22[] = "DEC(1)";
  static char var23[] = "DIV(2)";
  static char var24[] = "DEC(2)";
  static char var25[] = "DIV(3)";
  static char var26[] = "DEC(3)";
  static char var27[] = "DIV(4)";
  static char var28[] = "DEC(4)";
  static char var29[] = "DIV(5)";
  static char var30[] = "DEC(5)";
  static char var31[] = "DIV(6)";
  static char var32[] = "DEC(6)";
  static char var33[] = "DIV(7)";
  static char var34[] = "DEC(7)";
  static char var35[] = "DIV(8)";
  static char var36[] = "DEC(8)";
  static char var37[] = "DIV(9)";
  static char var38[] = "DEC(9)";
  static char var39[] = "DIV(10)";
  static char var40[] = "DEC(10)";
  static char var41[] = "DIV(11)";
  static char var42[] = "DEC(11)";
  static char var43[] = "DIV(12)";
  static char var44[] = "DEC(12)";
  static char var45[] = "DIV(13)";
  static char var46[] = "DEC(13)";
  static char var47[] = "DIV(14)";
  static char var48[] = "DEC(14)";
  static char var49[] = "DIV(15)";
  static char var50[] = "DEC(15)";
  static char var51[] = "DIV(16)";
  static char var52[] = "DEC(16)";
  static char var53[] = "DIV(17)";
  static char var54[] = "DEC(17)";
  static char var55[] = "DIV(18)";
  static char var56[] = "DEC(18)";
  static char var57[] = "DIV(19)";
  static char var58[] = "DEC(19)";
  static char var59[] = "buttons_menu";
  static char var60[] = "min_transmit_altitude";
  static char var61[] = "parallels_angle";
  static char var62[] = "equatorial_exclusion_angle";

  unsigned int test;

  /* add directory of tcl code */
  prepend_to_auto_path(TCL_DIR);

  /* add special commands */
  Tcl_CreateCommand(interp, command0, (Tcl_CmdProc *) satCmd, (ClientData) NULL,
		    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, command1, (Tcl_CmdProc *) satCmd, (ClientData) NULL,
		    (Tcl_CmdDeleteProc *) NULL);

  /* get info on some commands for optimization */
  Tcl_GetCommandInfo(interp, update, &update_info);

  /* link some variables */
  Tcl_LinkVar(interp, var0, (char *) &delta_t, TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp, var1, (char *) &coverage_angle, TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp, var2, (char *) &Image_Width, TCL_LINK_INT);
  Tcl_LinkVar(interp, var3, (char *) &Image_Height, TCL_LINK_INT);
  Tcl_LinkVar(interp, var4, (char *) &(pcb->radius), TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp, var5, (char *) &(pcb->mu), TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp, var6, (char *) &(pcb->rotation_rate), TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp, var7, (char *) &(pcb->J2), TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp, var8, (char *) &(pcb->orbital_period), TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp, var9, (char *) &(pcb->tilt), TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp, var10, (char *) &tracks_interval, TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp, var11, (char *) &Fisheye_Diameter, TCL_LINK_INT);
  Tcl_LinkVar(interp, var12, (char *) &NUM_COLORS, TCL_LINK_INT);
  Tcl_LinkVar(interp, var13, (char *) &coverage_projection, TCL_LINK_INT);
  Tcl_LinkVar(interp, var14, (char *) &Longitude_Center_Line, TCL_LINK_INT);
  Tcl_LinkVar(interp, var15, (char *) &coverage_display_center_longitude, TCL_LINK_INT);
  Tcl_LinkVar(interp, var16, (char *) &first_filename, TCL_LINK_STRING | TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp, var17, (char *) &debug, TCL_LINK_INT | TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp, var18, (char *) &splash_about, TCL_LINK_INT | TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp, var19, (char *) &DIV0, TCL_LINK_INT);
  Tcl_LinkVar(interp, var20, (char *) &DEC0, TCL_LINK_INT);
  Tcl_LinkVar(interp, var21, (char *) &DIV1, TCL_LINK_INT);
  Tcl_LinkVar(interp, var22, (char *) &DEC1, TCL_LINK_INT);
  Tcl_LinkVar(interp, var23, (char *) &DIV2, TCL_LINK_INT);
  Tcl_LinkVar(interp, var24, (char *) &DEC2, TCL_LINK_INT);
  Tcl_LinkVar(interp, var25, (char *) &DIV3, TCL_LINK_INT);
  Tcl_LinkVar(interp, var26, (char *) &DEC3, TCL_LINK_INT);
  Tcl_LinkVar(interp, var27, (char *) &DIV4, TCL_LINK_INT);
  Tcl_LinkVar(interp, var28, (char *) &DEC4, TCL_LINK_INT);
  Tcl_LinkVar(interp, var29, (char *) &DIV5, TCL_LINK_INT);
  Tcl_LinkVar(interp, var30, (char *) &DEC5, TCL_LINK_INT);
  Tcl_LinkVar(interp, var31, (char *) &DIV6, TCL_LINK_INT);
  Tcl_LinkVar(interp, var32, (char *) &DEC6, TCL_LINK_INT);
  Tcl_LinkVar(interp, var33, (char *) &DIV7, TCL_LINK_INT);
  Tcl_LinkVar(interp, var34, (char *) &DEC7, TCL_LINK_INT);
  Tcl_LinkVar(interp, var35, (char *) &DIV8, TCL_LINK_INT);
  Tcl_LinkVar(interp, var36, (char *) &DEC8, TCL_LINK_INT);
  Tcl_LinkVar(interp, var37, (char *) &DIV9, TCL_LINK_INT);
  Tcl_LinkVar(interp, var38, (char *) &DEC9, TCL_LINK_INT);
  Tcl_LinkVar(interp, var39, (char *) &DIV10, TCL_LINK_INT);
  Tcl_LinkVar(interp, var40, (char *) &DEC10, TCL_LINK_INT);
  Tcl_LinkVar(interp, var41, (char *) &DIV11, TCL_LINK_INT);
  Tcl_LinkVar(interp, var42, (char *) &DEC11, TCL_LINK_INT);
  Tcl_LinkVar(interp, var43, (char *) &DIV12, TCL_LINK_INT);
  Tcl_LinkVar(interp, var44, (char *) &DEC12, TCL_LINK_INT);
  Tcl_LinkVar(interp, var45, (char *) &DIV13, TCL_LINK_INT);
  Tcl_LinkVar(interp, var46, (char *) &DEC13, TCL_LINK_INT);
  Tcl_LinkVar(interp, var47, (char *) &DIV14, TCL_LINK_INT);
  Tcl_LinkVar(interp, var48, (char *) &DEC14, TCL_LINK_INT);
  Tcl_LinkVar(interp, var49, (char *) &DIV15, TCL_LINK_INT);
  Tcl_LinkVar(interp, var50, (char *) &DEC15, TCL_LINK_INT);
  Tcl_LinkVar(interp, var51, (char *) &DIV16, TCL_LINK_INT);
  Tcl_LinkVar(interp, var52, (char *) &DEC16, TCL_LINK_INT);
  Tcl_LinkVar(interp, var53, (char *) &DIV17, TCL_LINK_INT);
  Tcl_LinkVar(interp, var54, (char *) &DEC17, TCL_LINK_INT);
  Tcl_LinkVar(interp, var55, (char *) &DIV18, TCL_LINK_INT);
  Tcl_LinkVar(interp, var56, (char *) &DEC18, TCL_LINK_INT);
  Tcl_LinkVar(interp, var57, (char *) &DIV19, TCL_LINK_INT);
  Tcl_LinkVar(interp, var58, (char *) &DEC19, TCL_LINK_INT);
  Tcl_LinkVar(interp, var59, (char *) &buttons_menu, TCL_LINK_INT);
  Tcl_LinkVar(interp, var60, (char *) &min_transmit_altitude, TCL_LINK_INT);
  Tcl_LinkVar(interp, var61, (char *) &parallels_angle, TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp, var62, (char *) &equatorial_exclusion_angle, TCL_LINK_DOUBLE);

  /* execute initialization */
  {
    static char buf0_0[] = "set geomview_module 0";
    static char buf0_1[] = "set geomview_module 1";
    static char buf1[] = "app_init";

    if (geomview_module) {
      test = tcl_script(buf0_1);
    } else {
      test = tcl_script(buf0_0);
    }
    if (test) {
      test = tcl_script(buf1);
    }
  }

  return test;
}

/*
 * autopath is the directory list variable which tcl uses to
 * look for unknown commands
 */
static unsigned int
prepend_to_auto_path(const char path[])
{

  sprintf(cmd, "set auto_path [linsert $auto_path 0 %s]", path);
  return tcl_script(cmd);
}


/*
 * tcl_script
 *
 * execute script in tcl
 */
unsigned int
tcl_script(char script[])
{
  if (Tcl_GlobalEval(interp, script) == TCL_ERROR) {
#if (TK_MAJOR_VERSION >= 8)
    error_format("tcl_error: %s", Tcl_GetStringResult(interp));
#else
    error_format("tcl_error: %s", interp->result);
#endif
    return FALSE;
  }
  return TRUE;
}


/*
 * tk_update
 *
 * Routine to process tk events
 *
 */
unsigned int
tk_update()
{
  static char update[] = "update";
  const char *argv[1];
  int argc = 1;

  argv[0] = update;
  (*update_info.proc) (update_info.clientData, interp, argc, argv);

#if ( (TK_MAJOR_VERSION > 4) || ( (TK_MAJOR_VERSION == 4) && (TK_MINOR_VERSION > 0)) )
  if (Tk_GetNumMainWindows() > 0)
    return TRUE;
#else
  if (tk_NumMainWindows > 0)
    return TRUE;
#endif
  return FALSE;
}


/*
 * returns ClientData for a tcl object
 */
ClientData get_ClientData(char *obj)
{
  Tcl_CmdInfo info;

  if (Tcl_GetCommandInfo(interp, obj, &info))
    return info.clientData;

  /*
   * A useful method for testing and debugging of Tcl is
   * to print bad object name strings obj to stderr here.
   */
  return NULL;
}
