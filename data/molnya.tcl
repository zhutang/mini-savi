#
# * THEORETICAL CONSTELLATIONS
# *
# * ELLIPTICAL SYSTEMS
# *
# * Molnya (Molniya, Russian: 'Lightning') high-latitude coverage
# *
# * Highly-elliptical orbits useful near apogee.
# *
# * Originally used since 1965 for Soviet defence satellites, and then
# * for television broadcast in 1967 using Orbita Molnya satellites.
# * Much of the Soviet Union lies at high latitudes, where satellites
# * in geostationary orbit are very low on the horizon, leading
# * to the name 'Gorizont', or Horizon, for a series of Soviet
# * geostationary satellites. Molnya satellites are higher in the
# * sky, and more easily seen.
# *
# * Three equally-spaced satellites are required to give coverage
# * all the time at the desired longitude. Spacing isn't just in the
# * same orbit, since that gives apogees at different longitudes.
# * Instead, we must also space the right angles of the ascending nodes
# * (RAAN), so that different apogees are at the same longitude as the
# * Earth rotates.
# *
# * Information from 'Satellite Communication Systems', Maral and
# * Bousqet, 2nd/3rd edition, chapter 7; values are set to match
# * Figure 7.12.
# * 
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: molnya.tcl,v 1.19 2015/08/31 01:29:33 lloydwood Exp $


# setup orbital elements
# apogee longitude of slow-moving subsatellite point
# at a latitude of around 50-63.45 degrees.
# positive is east
set apogee_longitude 50.0

set NUM_PLANES 3

# constant semi-major axis for these orbits
set sma 26556.0

# eccentricity can be from 0.6 to 0.75
set e 0.71

set perigee_altitude [expr ($sma*(1-$e)-$RADIUS_OF_EARTH)]
set apogee_altitude  [expr ($sma*(1+$e)-$RADIUS_OF_EARTH)]

# mean altitude is semi-major axis
set a $sma

# * Setting minimum transmission altitude to 20,000 km works well
# * to turn off transmission away from apogee with some overlap.
# *
# * savi -min-transmit-altitude 20000
# or uncomment:
# set min_transmit_altitude 20000



# inclination must be  63.435 (stationary apogee in northern hemisphere)
#                  or -63.435 (stationary apogee in southern hemisphere)
set inc 63.435

set omega 270.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# effective limit of coverage
set coverage_angle 5.0


satellites GV_BEGIN

for {set j 0} {$j < $NUM_PLANES} {incr j} {
   # three planes are spaced by 120 degrees
   set Omega [expr $j * 360.0 / $NUM_PLANES]

   # orbital positions are spaced by 120 degrees.
   # orbits are twelve-hour.
   set T [expr ($T_per * -(2*$apogee_longitude+$Omega)/360) ]
   satellites LOAD $a $e $inc $Omega $omega $T "Molyna-$j"
}

satellites GV_END
