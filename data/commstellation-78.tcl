#
# * FAT PIPE DREAMS
# *
# * COMMstellation
# *
# * Announced January 2011, with some initial information on
# * orbital parameters. It uses a Walker polar star geometry,
# * but with a stated distance between seams of thirty degrees
# * for six planes there is no adjustment for the counter-rotating
# * seam. Minimum elevation angle (10 degrees is assumed, though with
# * Walker seam adjustment this could change), phasing (ABAB is
# * assumed) and inclination of orbit (90 degrees assumed) are not
# * stated.
# *
# * This is a very approximate simulation, and likely to be altered
# * as futher details emerge.
# *
# * See http://www.commstellation.com/ and http://www.mscinc.ca/
#
# most simulation details from:
# http://www.commstellation.com/constellation/coverage.html
#
# $Id: commstellation-78.tcl,v 1.3 2011/02/11 08:02:57 lloydwood Exp $

set SATS_PER_PLANE 12
set NUM_PLANES 6
set INTERPLANE_SPACING 30.0

# setup orbital elements
set a [expr 1000.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 90.0
set omega 0.0

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
		set n [satellites LOAD $a $e $inc $Omega $omega $T "COMMstellation ($j, $i)"]
		if {$i > 0} {satellites ORBIT_SET $n 0}
	}
}
satellites GV_END

