#
# * OPERATIONAL CONSTELLATIONS
# *
# * Disaster Monitoring Constellation (DMC), circa 2006.
# *
# * A satellite constellation designed at the University of Surrey,
# * and built by Surrey Satellite Technology Ltd.
# *
# * Four satellites are spaced equally in a sun-synchronous plane,
# * which ascends over the Equator at 10:15 am, in order after one
# * another:
# * Alsat-1, NigeriaSat-1, BILSAT-1, UK-DMC. Alsat-1 was launched in
# * November 2002, while the other three were launched in September
# * 2003.
# *
# * The fifth satellite, Beijing-1, launched October 2005, is in a
# * separate plane which ascends over Equator at 10:30am.
# *
# * This script turns on sunlight to show the sun-synchronous orbits.
# *
# * Orbital details from a presentation on the DMC and its
# * capabilities:
# * David Hodgson, 11th Conference on Control with Remote Sensing of
# * Area-based Subsidies, Krakow, Poland, 25 November 2005 (slide 5).
# *
# * This simulation has not yet been updated to reflect the later
# * launches of:
# * - UK-DMC2 and Deimos-1 on 29 July 2009.
# * - NigeriaSat-2 and NigeriaSat-X on 17 August 2011.
# *
# * BilSAT-1 is no longer operational due to batteries no longer
# * charging. UK-DMC has now reached end of life, and is no longer
# * in operational use.
# *
# * See also http://www.dmcii.com/.
# *
# * These satellites use the Internet Protocol to communicate. The
# * UK-DMC satellite has acted as a networking testbed with an onboard
# * Internet router from Cisco Systems, and became the first satellite
# * to run IPv6 and the Delay-Tolerant Networking 'Bundle Protocol'.
#
# $Id: dmc.tcl,v 1.18 2015/01/15 23:03:13 lloydwood Exp $

# sun-synchronous, so turn on sunlight
upvar #0 sun_flag sun_flag
set sun_flag 1

set a [expr 686.0+$RADIUS_OF_EARTH]

set inc 98.2
set e 0.0
set omega 0.0

# ascending over Equator at 10:15
set Omega [expr 10.25/12 * 180.0]

# 0 or 5 degrees would be minimum for ground station visibility.
# To get an idea of 600km swath width under DMC satellite,
# arctan(686/300) = 66.4 degrees - ignores curvature of Earth.
# or 23.6 for half-cones from satellites.
# We could set half-cone for correct fisheye behaviour, but then
# the popup and subsequently-loaded scripts are incorrect.

set coverage_angle 66.4

# set coverage_angle 23.6
# upvar #0 coverage_angle_flag coverage_angle_flag
# set coverage_angle_flag 1
# puts stderr "\n\nSaVi: half-cone coverage angle of $coverage_angle deg shows extent of DMC imaging swath."

puts stderr "\nSaVi: imaging swath shown. Set mask angle to zero for limits of communications connectivity."

set SATS_PER_PLANE 4

# compute period of orbit
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

satellites GV_BEGIN

for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
    set T [expr $T_per / $SATS_PER_PLANE * $i ]
    satellites LOAD $a $e $inc $Omega $omega $T "DMC ($i)"
}

# 15 minutes difference is 360/24/4 = 3.75 degrees west.
set Omega [expr $Omega + 3.75]

# optimum intermediate spacing would interleave the satellite with
# the other plane; Beijing-1 likely isn't this well-spaced.

set T [expr $T_per / $SATS_PER_PLANE /2 ]

satellites LOAD $a $e $inc $Omega $omega $T "DMC (Beijing-1)"

satellites GV_END
