#
# * PROPOSED LARGE BROADBAND CONSTELLATIONS - FAT PIPE DREAMS
# *
# * SkyBridge double-delta constellation
# *
# * This system, developed by Alcatel, was originally called Sativod
# * (for Satellite Video on Demand).
# * 
# * This is the original 64-active-satellite proposal, which consists
# * of two overlapping rosettes, slightly offset. The first
# * subconstellation was originally planned for launch in 2000, with
# * the second in 2002. These were redesigned in 1998 to a single
# * 80-active-satellite constellation.
# *
# * The aim of having two overlapping synchronised rosettes was to give
# * frequency sharing with satellites in GEO. The idea was that when
# * one Skybridge satellite was too near in the sky to the equatorial
# * belt and to a geostationary satellite, service could be provided
# * by its slightly-more-distant neighbour. This can be demonstrated
# * by showing the equatorial exclusion zone.
# *
# * See:
# *  the SkyBridge FCC application, 28 February 1997, pp. 27-31.
# *
# * Jean-Luc Palmade, Eric Frayssinhes and Erick Lansard, Constellation
# * of non-geostationary satellites providing permanent coverage,
# * US patent 6032902, issued 7 March 2000 to Alcatel Espace.
# http://www.patentstorm.us/patents/6032902.html

# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: skybridge-64.tcl,v 1.14 2015/08/31 01:29:33 lloydwood Exp $



# Elevation angle apparently 10 degrees, but that's not quite full
# coverage.
set coverage_angle 10.0

# I'm not convinced by the orbital parameters given - they define the
# argument of perigee as 90 degrees, and the eccentricity as 0 degrees
# (table, p.30). Some redundancy, surely? Would have been impossible
# to figure out if they hadn't given a planar projection of satellite
# distribution; it's continuous-phasing.

set SATS_PER_PLANE 4
set NUM_PLANES 8

# set to one to show the first subconstellation after completed launch
set NUM_CONSTS 2

# offset of second SkyBridge sub-constellation, degrees
set LONG_OFFSET 10
set LAT_OFFSET -14

# setup orbital elements
set a [expr 1457.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 55
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

puts stderr "\nSaVi: showing Skybridge's equatorial exclusion zone."
upvar #0 plane_flag plane_flag
set plane_flag 1

satellites GV_BEGIN

# outer loop for both sub-constellations
for {set k 0} {$k < $NUM_CONSTS} {incr k} {

   for {set j 0} {$j < $NUM_PLANES} {incr j} {

        set Omega [expr $j * 45.0 + $k * $LONG_OFFSET ]
        set plane_offset [expr $T_per / 360.0 * ($j * -33.75 + $k * $LAT_OFFSET) ]

        for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {

                set T [expr $T_per * $i / $SATS_PER_PLANE + $plane_offset]
		satellites LOAD $a $e $inc $Omega $omega $T "SkyBridge-64 ($j, $i, $k)"
	}
    }
}
satellites GV_END
