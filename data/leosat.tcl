#
# * PROPOSED LARGE BROADBAND CONSTELLATIONS - CURRENT EFFORTS
# *
# * LeoSat
# *
# * A Walker polar star geometry, using optical intersatellite
# * links with the Iridium NEXT bus. Note the 'orbital seam' where
# * the footprints of ascending (going north over the Equator) and
# * descending (going (south) satellites overlap more to ensure
# * continuous coverage.
# *
# * See Petition for Declatory Ruling, LeoSat MA, Inc., US FCC
# * SAT-LOI-20161115-00112, 15 November 2016.
# *
# * "It's 78-108 satellites in polar orbit at 1,430 kilometers in
# * altitude, six orbital planes with 18 satellites each."
# * -- Mark Rigolle, CEO of LeoSat, interviewed by Peter B. de Selding
# * in "Never Mind the Unconnected Masses, LeoSat’s Broadband Constellation
# * is Strictly Business," Space News, 20 November 2015.
# http://spacenews.com/nevermind-the-unconnected-masses-leosats-broadband-constellation-is-strictly-business/
# *
# * See http://leosat.com/
# *
# * Uses intersatellite links, which are not yet simulated here.

# $Id: leosat.tcl,v 1.6 2016/11/19 07:40:56 lloydwood Exp $

set SATS_PER_PLANE 18
set NUM_PLANES 6

set INTERPLANE_SPACING 31.0

# setup orbital elements
# could be retrograde sun synchronous orbit to simplify solar panel
# tracking, at circa 110 degrees, but seems unlikely.
set a [expr 1430.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 87.0
set omega 0.0

# needs relatively low elevation angle for terminals.
# Technical Annex says 10 degrees, though 25 gives full coverage.
set coverage_angle 10.0

# compute period of orbit
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]


satellites GV_BEGIN
for {set j 0} {$j < $NUM_PLANES} {incr j} {
	set Omega [expr $j * $INTERPLANE_SPACING]
	for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
		if { $j % 2 == 0} {
			set plane_offset 0
		} else {
			set plane_offset [expr $T_per / $SATS_PER_PLANE / 2.0]
		}
		set T [expr $T_per * $i / $SATS_PER_PLANE + $plane_offset]
		set n [satellites LOAD $a $e $inc $Omega $omega $T "LeoSat ($j, $i)"]
		if {$i > 0} {satellites ORBIT_SET $n 0}
	}
}
satellites GV_END

