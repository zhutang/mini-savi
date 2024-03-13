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
 * tcl_utils.h
 *
 * $Id: tcl_utils.h,v 1.21 2017/01/02 06:11:32 lloydwood Exp $
 */

#ifndef _TCL_UTILS_H_
#define _TCL_UTILS_H_

/* necessary to tell Tcl 8.6 to be backwards-compatible
 * and support interp->result, needed in satCmd.c::satCmd()
 * and also used in non-critical debug error statements.
 * Tcl_GetStringResult() / Tcl_SetResult() don't work in 7.x.
 * http://www.tcl.tk/man/tcl8.6/TclLib/Interp.htm
 */
#ifndef USE_INTERP_RESULT
#define USE_INTERP_RESULT
#endif

#ifndef _TK
#include <tk.h>
#endif

Tcl_Interp *interp; /* Interpreter for this application. */

unsigned int tcl_init(Tcl_Interp * interpreter);
unsigned int tk_init(Tcl_Interp * interpreter);

unsigned int tcl_script(char []);
unsigned int tk_update(void);

extern int satCmd(ClientData, Tcl_Interp *, int, char *[]);
extern ClientData get_ClientData(char *);


#endif
/* !_TCL_UTILS_H_ */
