#
######################################################
#
#  SaVi by Lloyd Wood (lloydwood@users.sourceforge.net),
#          Patrick Worfolk (worfolk@alum.mit.edu) and
#          Robert Thurman.
#
#  Copyright (c) 1997 by The Geometry Center.
#  Also Copyright (c) 2017 by Lloyd Wood.
#
#  This file is part of SaVi.  SaVi is free software;
#  you can redistribute it and/or modify it only under
#  the terms given in the file COPYRIGHT which you should
#  have received along with this file.  SaVi may be
#  obtained from:
#  http://savi.sourceforge.net/
#  http://www.geom.uiuc.edu/locate/SaVi
#
######################################################
#
# sunlight.tcl
#
# $Id: sunlight.tcl,v 1.14 2017/01/02 06:11:33 lloydwood Exp $


# Sunlight on Earth
#
# We fake the Sun as a satellite taking a year to orbit Earth,
# with the same inclination as the ecliptic.
#
# This is intended to give a realistic day/night terminator
# in the global coverage panel.
# (Turning on interval decay to see the night in blue helps.)

proc sunlight(create) {} {

set PI 3.14159

# valid for Earth's gravitational field.
set MU 398601.2

# set up orbital elements

# CHECK EARTH'S ORBIT!
# Which way round the Sun does the Earth rotate?
# (See Orrery - should be anticlockwise when viewed from north pole.)
# Which way round the Earth does this solar 'satellite' rotate?
# (should also be anticlockwise, but the Earth is spinning
#  hundreds of times faster -- and anticlockwise -- making it
#  look clockwise.)

# Radius of Earth's orbit is 150 million km, more or less --
# Semimajor axis (10^6 km)         149.60
# Sidereal orbit period (days)    365.256
# Tropical orbit period (days)    365.242
# Perihelion (10^6 km)             147.09
# Aphelion (10^6 km)               152.10
# but that's only meaningful for a year's period when in the Sun's
# gravitational field, which we're not. So we must fake up an
# altitude to give us the same period from an orbit in the Earth's
# gravitational field instead, by manipulating the classic:
# set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]
# equation to get what we want with the MU and period
# we have to use.

# We can use these:
# Earth Mean Orbital Elements (J2000)
#
# Semimajor axis (AU)                  1.00000011
# Orbital eccentricity                 0.01671022
# Orbital inclination (deg)            0.00005
# Longitude of ascending node (deg)  -11.26064
# Longitude of perihelion (deg)      102.94719
# Mean Longitude (deg)               100.46435
# 1 AU = 149,597,870.691 kilometres
# data from NASA Earth fact sheet:
#
# http://nssdc.gsfc.nasa.gov/planetary/factsheet/earthfact.html


# set period of a year.
set T_per [expr 360.25 * 24 * 60 * 60]
set a1 [expr $T_per * sqrt($MU) / ( 2 * $PI) ]
set a [expr pow($a1,0.666666667)]

# Inclination is angle between planes of equator and ecliptic.
set inc 23.45

# Eccentricity of 0 is an approximation --
# if we set this to anything other than zero, we'll
# have considerable trouble with getting omega right
# in our modified environment for a given year.

# real Earth orbit has eccentricity of 0.0167
set e 0.0

set omega 0.0
set Omega 0.0

# Special-casing coverage as 0 is set in C code.

# We want to start with equinox on the international date line.
# Matches behaviour of SaVi 1.1 where Sun starts as fourth axis.
set T [expr -$T_per/2]

# Disabling visibility in Geomview is a nice touch;
# avoiding zooming out when you press Look is useful.
set disp_satellite 0
set disp_coverage 1
set disp_orbit 0

# Prevent Geomview from drawing while we load in the satellites.
satellites GV_BEGIN

set sat [satellites LOAD $a $e $inc $Omega $omega $T "sunlight"]

# below line needs SaVi > 1.0 for all trailing parameters
satellites SET $sat $a $e $inc $Omega $omega $T \
	$disp_satellite $disp_orbit $disp_coverage

satellites GV_END

}
