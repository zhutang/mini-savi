#
# * ELLIPTICAL SYSTEMS
# *
# * Sirius Radio
# *
# * A three-satellite Molnya-like constellation which uses modified
# * Tundra orbits.
# *
# * Highly-elliptical orbits useful from apogee.
# *
# * Sirius Radio later merged with XM Radio, which offered a competing
# * service from geostationary orbit.
#
# * See http://www.sirius.com/.
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: sirius-radio.tcl,v 1.14 2015/08/31 01:29:33 lloydwood Exp $

# information from 'mission overview' Acrobat pdf handout found at:
# http://www.ilslaunch.com/launch_schedules/overviews/sirius2.pdf
# in September 2000.
# Further information at http://www.siriusradio.com/

# Table 1.2 from that handout:
# inclination                   63.4 +/- 0.5
# eccentricity                  0.2684 +/- 0.005
# orbit period                  24 hours
# relative phasing              8 hours +/- 10 minutes
# semi-major axis               42,164 km
# pedigree (sic!) radius        24,469 km
# apogee radius                 47,102 km
# RAAN                          45.0, 165.0 and 285.0
# argument of perigee           270.0 +/- 0.5
# apogee longitude              96.0W +/- 0.5
# mean anomaly Y=f(RAAN)        Y-120, Y-290


# setup orbital elements
# indicate longitude of slow-moving subsatellite point
# at approximate latitude of 60 degrees.
# east is positive
set apogee_longitude -96.0

# two satellites in separate 24-hour Tundra orbits are needed
# for full, continuous coverage of the chosen area.
# Sirius Radio uses three, giving it Molnya-like qualities.
set SATS_PER_PLANE 3

# constant semi-major axis for these orbits
set sma 42164.0

# eccentricity can vary from 0.25 to 0.4
# top loop vanishes at 0.37
set e 0.2684

set perigee_altitude [expr ($sma*(1-$e)-$RADIUS_OF_EARTH)]
set apogee_altitude [expr ($sma*(1+$e)-$RADIUS_OF_EARTH)]

set a $sma

# inclination must be  63.435 for stationary apogee in northern hemisphere
# invert sign for southern hemisphere.
set inc 63.4

# switch sign around to ensure east is positive
set apogee_longitude -$apogee_longitude

# 270 degrees gives an optimal orbit
# to vary latitude, you need to vary omega and e.
set omega 270.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# zero is 90 degrees west, due to omega
set offset_longitude 90
set apogee_longitude [expr ($apogee_longitude-$offset_longitude)]

# effective limit of coverage is said to be 60 degrees, but that's
# extremely high, and rarely gives double coverage.
# We'll need >30 to ensure continuous double
# coverage over the continental United States.
set coverage_angle 30.0

satellites GV_BEGIN

# note introduction of 45 for RAANs at 45+0, 45+120, 45+240.

for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
   set Omega [expr ($i*120)+45-$apogee_longitude ]
   set T [expr ($T_per * ($i*120+45)/360 ) ]
   satellites LOAD $a $e $inc $Omega $omega $T "Sirius-Radio-$i"
}

satellites GV_END
