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
 * int_types.h
 *
 * $Id: int_types.h,v 1.11 2017/01/02 06:11:32 lloydwood Exp $
 */

#ifndef _INT_TYPES_H_
#define _INT_TYPES_H_

/* traditional */
#include <sys/types.h>

/* modern C99 definitions file not used by many compilers. */
#include <stdint.h>

/*
 * If the modern uint16_t type isn't included, map it
 * to the older u_int16_t type from BSD, which is likely to
 * exist instead. Ditto for uint32_t -> u_int32_t.
 */

#ifndef uint16_t
#define uint16_t u_int16_t
#endif

#ifndef uint32_t
#define uint32_t u_int32_t
#endif

#endif
/* !_INT_TYPES_H_ */
