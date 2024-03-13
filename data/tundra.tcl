#
# * THEORETICAL CONSTELLATIONS
# *
# * ELLIPTICAL SYSTEMS
# *
# * Tundra high-latitude coverage - compare with similar Soviet Molnya.
# *
# * Highly-elliptical orbits useful near apogee.
# *
# * Two satellites in separate orbits are necessary for full,
# * continuous coverage of the chosen point.
# *
# * Information from 'Satellite Communication Systems', Maral and
# * Bousquet, 2nd/3rd edition, chapter 7; values are set to match
# * Figure 7.13.
# *
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: tundra.tcl,v 1.16 2015/08/31 01:29:33 lloydwood Exp $


# setup orbital elements
# indicate longitude of slow-moving subsatellite point
# at approximate latitude of 60 degrees.
# east is positive
set apogee_longitude 50

set SATS_PER_PLANE 2

# constant semi-major axis for these orbits
set sma 42164.0

# eccentricity can vary from 0.25 to 0.4
# top loop vanishes at 0.37
set e 0.4

set perigee_altitude [expr ($sma*(1-$e)-$RADIUS_OF_EARTH)]
set apogee_altitude [expr ($sma*(1+$e)-$RADIUS_OF_EARTH)]

set a $sma

# * Setting minimum transmission altitude to 40,000 km works well
# * to turn off transmission away from apogee with some overlap.
# *
# * savi -min-transmit-altitude 40000
# or uncomment:
# set min_transmit_altitude 40000


# inclination must be  63.435 (stationary apogee in northern hemisphere)
#                  or -63.435 (stationary apogee in southern hemisphere)
set inc 63.435

# switch sign around to ensure east is positive
set apogee_longitude -$apogee_longitude

# 270 degrees gives an optimal orbit
# to vary latitude, you need to vary omega and e.
set omega 270.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# zero is 90 degrees west
set offset_longitude 90
set apogee_longitude [expr ($apogee_longitude-$offset_longitude)]

# effective limit of coverage
set coverage_angle 5.0

satellites GV_BEGIN

for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
   set Omega [expr $i*180-$apogee_longitude]
   set T [expr $T_per * $i / $SATS_PER_PLANE ]
   satellites LOAD $a $e $inc $Omega $omega $T "Tundra-$i"
}

satellites GV_END
