#
# * PROPOSED LARGE BROADBAND CONSTELLATIONS - CURRENT EFFORTS
# *
# * OneWeb
# *
# * Greg Wyler's followup to the popular O3b.
# *
# * A Walker polar star geometry. Note the 'orbital seam' where
# * the footprints of ascending (going north over the Equator) and
# * descending (going (south) satellites overlap more to ensure
# * continuous coverage.
# *
# * This simulation is a rough approximation, based only on news reports.
# * It is speculative, not authoritative, and not finalised.
# *
# * "OneWeb’s 150-kilogram satellites will operate in an orbit
# * 1,200 kilometers in altitude, in 18 planes of 40 satellites each,
# * with an inclination of 87.9 degrees relative to the equator."
# *  -- OneWeb Pledges Vigilance on Orbital Debris Issue,
# * Peter B. de Selding, October 15, 2015.
# http://spacenews.com/oneweb-pledges-vigilance-on-orbital-debris-issue/

# * See http://oneweb.world/ or http://spacenews.com/tag/oneweb/
# *
# * Uses intersatellite links, which are not yet simulated here.

# $Id: oneweb.tcl,v 1.4 2016/01/18 03:16:52 lloydwood Exp $

set SATS_PER_PLANE 40
set NUM_PLANES 18

# rough guess.
set INTERPLANE_SPACING 10.0

# setup orbital elements
set a [expr 1200.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 87.9
set omega 0.0

# Ku-band, but presume high elevation angle for terminals.
set coverage_angle 50.0

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
		set n [satellites LOAD $a $e $inc $Omega $omega $T "OneWeb ($j, $i)"]
		if {$i > 0} {satellites ORBIT_SET $n 0}
	}
}
satellites GV_END

