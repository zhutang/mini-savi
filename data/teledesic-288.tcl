#
# * PROPOSED LARGE BROADBAND CONSTELLATIONS - FAT PIPE DREAMS
# *
# * Teledesic (288-active satellite Boeing design)
# *
# * A reduced number of satellites, at higher altitude, than the
# * first 840-active-satellite proposal.
# *
# * Uses intersatellite links, which are not yet simulated here.
# *
# * Based on a Boeing-led redesign before a merger with Motorola's
# * competing Celestri effort in May 1998, which was then followed by
# * a planned 30-active-satellite MEO system announced in February
# * 2002. 
# *
# * In October 2002 Teledesic suspended satellite work, and in July
# * 2003 surrendered allocated frequencies.
# *
# * Craig McCaw, who had funded Teledesic development, then invested in
# * the company developing the smaller MEO ICO system.
# 
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: teledesic-288.tcl,v 1.10 2015/08/31 01:29:33 lloydwood Exp $


# note that planes would not be as clearly offset in reality;
# with such a large number of satellites, control of phasing
# is hard and random phasing between planes is likely to result.
# Coverage should cope with that.

# Eccentricity not known; approximated to zero.

set SATS_PER_PLANE 24
set NUM_PLANES 12

# setup orbital elements
set a [expr 1375.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 84.7
set omega 0.0

# still Ka-band and 40 degrees mask elevation.
set coverage_angle 40.0

# compute period of orbit
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

satellites GV_BEGIN
for {set j 0} {$j < $NUM_PLANES} {incr j} {

	# space planes evenly, adjusted for seam.
	set Omega [expr $j * 15 ]

	for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
		if { $j % 2 == 0} {
			set plane_offset 0
		} else {
			set plane_offset [expr $T_per / $SATS_PER_PLANE / 2.0]
		}
                set T [expr $T_per * ($i + $j/double($NUM_PLANES)) / \
					$SATS_PER_PLANE + $plane_offset ]

	    satellites LOAD $a $e $inc $Omega $omega $T "Teledesic-1997 ($j, $i)"
	}
}
satellites GV_END

