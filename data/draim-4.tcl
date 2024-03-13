#
# * THEORETICAL CONSTELLATIONS
# * ELLIPTICAL SYSTEMS
# *
# * Draim example 4-satellite elliptical constellation
# *
# * Uses elliptical orbits for continuous coverage.
# *
# * If the Earth is completely enclosed within a tetrahedron,
# * then satellites at the vertices of that tetrahedron can
# * see the whole surface of the Earth.
# *
# * John E. Draim, A common-period four-satellite continuous
# * global coverage constellation, AIAA Journal of Guidance,
# * Control and Dynamics, vol. 10, no. 5, September-October 1987,
# * pp. 492-499. Orbital parameters from Table 1.
# *
# * John E. Draim, Tetrahedral multi-satellite continuous-coverage
# * constellation, US Patent 4,854,527, August 1989.
# http://www.freepatentsonline.com/4854527.html
#
# $Id: draim-4.tcl,v 1.13 2014/01/28 21:00:09 lloydwood Exp $

set NUM_PLANES 4

# apogee altitude 42203 nautical miles - translate to km
set apogee_altitude 78160.0

# perigee altitude 23193 nautical miles - translate to km
set perigee_altitude 42953.5

# setup orbital elements
set a [expr ($apogee_altitude+$perigee_altitude)/2+$RADIUS_OF_EARTH]

# e is approximately 0.263
set e [expr ($apogee_altitude-$perigee_altitude)/(2*$a)]

set inc 31.3
set omega -90

set coverage_angle 0.0

# compute period of orbit
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

satellites GV_BEGIN
for {set j 0} {$j < $NUM_PLANES} {incr j} {
    set Omega [expr $j * 360.0 / $NUM_PLANES]
    set omega [expr -$omega]
    set T [expr $T_per / $NUM_PLANES * $j ]
    satellites LOAD $a $e $inc $Omega $omega $T "Draim-$j"
}
satellites GV_END
