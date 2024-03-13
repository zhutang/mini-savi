#
# * Aries
# *
# * Constellation Communication Inc. was a joint venture between Raytheon,
# * Bell Atlantic, and others. Not built.
# *
# * See:
# *  R. A. Summers and R. J. Lepkowski, Aries - Global communication
# * through a constellation of low Earth orbit satellites,
# * Proceedings of the 14th AIAA International Communication Satellite
# * Systems Conference, March, 1992, pp. 628-638.

# $Id: aries.tcl,v 1.5 2008/12/29 00:31:26 lloydwood Exp $

set SATS_PER_PLANE 12
set NUM_PLANES 4

# setup orbital elements
set a [expr 1018+$RADIUS_OF_EARTH]
set e 0.0
set inc 90.0
set omega 0.0

# Uncertain of minimum elevation angle; set to zero.
set coverage_angle 0.0

# compute period of orbit
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

satellites GV_BEGIN
for {set j 0} {$j < $NUM_PLANES} {incr j} {
	set Omega [expr $j * 180.0 / $NUM_PLANES]
	for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
		if { $j % 2 == 0} {
			set plane_offset 0
		} else {
			set plane_offset [expr $T_per / $SATS_PER_PLANE / 2.0]
		}
		set T [expr $T_per * $i / $SATS_PER_PLANE + $plane_offset]
		set n [satellites LOAD $a $e $inc $Omega $omega $T "Aries ($j, $i)"]
		if {$i > 0} {satellites ORBIT_SET $n 0}
	}
}
satellites GV_END
