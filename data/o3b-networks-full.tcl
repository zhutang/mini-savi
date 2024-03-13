#
# * PROPOSED LARGE BROADBAND CONSTELLATIONS - CURRENT EFFORTS
# *
# * O3b additional rings for full global coverage
# *
# * See http://www.o3bnetworks.com/
# *
# * This is the future build-out described in O3b Ltd's November 2016
# * filing with the US FCC to add satellites in high-inclination
# * circular orbits to cover polar latitudes.
# *
# * See "Amendment to Application to Modify U.S. Market Access Grant
# * for the O3b Medium Earth Orbit Satellite System," O3b Limited,
# * US FCC SAT-AMD-20161115-00116, 15 November 2016.
# *
# * The other O3b script (o3b-networks.tcl) describes the existing
# * operational equatorial ring in detail. These satellites add to
# * that, so use Load... to bring the other script in and simulate
# * all satellites.

# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://savi.sourceforge.net/
#
# $Id: o3b-networks-full.tcl,v 1.2 2017/01/02 11:30:45 lloydwood Exp $

# full equatorial ring - this is the zeroth plane.
# 24 new satellites add to 12 in orbit and 8 already planned
# for original O3b constellation, but with four in-orbit spares
# for the new constellation, per A.9.1.1 p. 15 of amendment.
set NUM_SATS [expr 24 - 4]
set inc 0.0

# setup orbital elements
set a [expr 8062.0+$RADIUS_OF_EARTH]
set e 0.0
# may want to optimise staggering from original constellation
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# We're told coverage between 45 degrees of latitude.
# Ka-band, so a relatively high mask angle can be expected.
# But mask must be low to cover up to 45 deg lat consistently.
# > 5 degrees elevation for 7.3m diameter gateway stations.
# > 15 degrees elevation for 2.4m diameter and larger antennas.
# > 20 degrees elevation for 1.8m diameter antennas.

set coverage_angle 15.0

satellites GV_BEGIN

set T [expr $T_per ]
for {set j 0} {$j < $NUM_SATS} {incr j} {
	set Omega [expr $j * 360.0 / $NUM_SATS]
	satellites LOAD $a $e $inc $Omega $omega $T "O3bN (0, $j)"
}

# inclined orbits - two planes of eight each.
set inc 70.0
set SATS_PER_PLANE 8
set NUM_PLANES 2

# staggering planes slightly so satellites don't cross - use a prime!
set phase [expr $T_per / $SATS_PER_PLANE / 7]

for {set j 1} {$j <= $NUM_PLANES} {incr j} {
        set Omega [expr $j * 360.0 / $NUM_PLANES ]
        for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
                set T [expr $T_per * $i / $SATS_PER_PLANE + $phase * $j]
                satellites LOAD $a $e $inc $Omega $omega $T "O3bI ($j, $i)"
        }
}

satellites GV_END
