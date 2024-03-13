#
# * OPERATIONAL CONSTELLATIONS
# *
# * RapidEye
# *
# * Five remote-sensing satellites, spaced equally in a sun-synchronous
# * plane. Launched on a single rocket in August 2008.
# *
# * This script turns on sunlight to show the sun-synchronous orbits.
# *
# * Satellite platforms constructed by Surrey Satellite Technology Ltd
# * (SSTL).
# *
# * Orbits in the opposite direction to the Disaster Monitoring
# * Constellation, also built by SSTL.
# *
# * See also http://www.rapideye.de/.
#
# $Id: rapideye.tcl,v 1.8 2015/01/15 23:03:13 lloydwood Exp $

# sun-synchronous, so turn on sunlight
upvar #0 sun_flag sun_flag
set sun_flag 1

set a [expr 630.0+$RADIUS_OF_EARTH]

set inc 98.2
set e 0.0
set omega 0.0

# descending over Equator at 11am - ascends 11pm
set Omega [expr 23.0 / 24.0 * 360]

# 0 or 5 degrees would be minimum for ground station visibility.
# Imaging swath is 6.75 degrees about nadir, corresponding to a swath of
# over 70 km at an orbital altitude of 620 km
# arctan(630/35) = 86.82
# ignores curvature of Earth

set coverage_angle 86.82

# set coverage_angle 6.75
# upvar #0 coverage_angle_flag coverage_angle_flag
# set coverage_angle_flag 1
# puts stderr "\n\nSaVi: half-cone coverage angle of $coverage_angle deg shows extent of RapidEye imaging swath."

puts stderr "\nSaVi: imaging swath shown. Set mask angle to zero for limits of communications connectivity."

set SATS_PER_PLANE 5

# compute period of orbit
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

satellites GV_BEGIN

for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
    set T [expr $T_per / $SATS_PER_PLANE * $i ]
    satellites LOAD $a $e $inc $Omega $omega $T "RapidEye ($i)"
}

satellites GV_END
