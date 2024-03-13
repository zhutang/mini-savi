#
# * PROPOSED LARGE BROADBAND CONSTELLATIONS - FAT PIPE DREAMS
# *
# * SkyBridge (80 satellite final proposal)
# * 
# * The well-documented 64-satellite system with two overlapping
# * subconstellations was redesigned into this larger single rosette.
# *
# * In March 2001, reports of intent to use geostationary satellite
# * transponders appeared. SkyBridge was put on hold as of January
# * 2002.
# *
# * See:
# * Skybridge will expand its satellite constellation from 64 to 80
# * satellites, Alcatel press release, 1 June 1998.
# *
# * P. Fraise, B. Coulomb, B. Monteuuis and J.-L. Soula,
# * SkyBridge LEO satellites: optimized for broadband communications
# * in the 21st century, Proceedings of the IEEE Aerospace conference,
# * vol. 1 pp. 241-251.
#
# With thanks to Judith Cote for verifying phase offset information.
#
# $Id: skybridge-80.tcl,v 1.5 2011/04/27 09:02:38 lloydwood Exp $

set SATS_PER_PLANE 4
set NUM_PLANES 20

# setup orbital elements
set a [expr 1469.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 53.0
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# Coverage angle presumed the same as for previous design
# supports minimum dual diversity everywhere, at least.
set coverage_angle 10.0

# * The constellation phasing repeats every five planes.
# *  360/80 = 4.5. Ballard fifth harmonic is 4.5 * 5 = 22.5 degrees.

# This is a classic Ballard rosette, much simpler than the
# 1997 two-overlapping-rosette 64-satellite design.

satellites GV_BEGIN
for {set j 0} {$j < $NUM_PLANES} {incr j} {
	set Omega [expr $j * 360.0 / $NUM_PLANES]
	for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
		set T [expr $T_per * ( double($i) / $SATS_PER_PLANE - 22.5/360.0*$j) ]
		set n [satellites LOAD $a $e $inc $Omega $omega $T "SkyBridge-80 ($j, $i)"]
		if {$i > 0} {satellites ORBIT_SET $n 0}
	}
}
satellites GV_END
