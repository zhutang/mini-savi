#
# * Spaceway NGSO
# *
# * See Hughes Spaceway NGSO FCC filing, 22 December 1997.
# * SaVi output can be compared to Figure C-1, p69, Appendix C.
# *
# * Uses intersatellite links, which are not yet simulated here.
#
# Zero phasing between planes, since an odd number of sats in planes
# 180deg opposed means that sats cross the other plane halfway between
# sats in that plane, which is ideal for coverage. Just like ICO.
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: spaceway-ngso.tcl,v 1.9 2015/08/31 01:29:33 lloydwood Exp $

set SATS_PER_PLANE 5
set NUM_PLANES 4

# setup orbital elements
set a [expr 10352.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 55
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# elevation angle of 30 degrees
set coverage_angle 30.0

satellites GV_BEGIN
for {set j 0} {$j < $NUM_PLANES} {incr j} {
	set Omega [expr $j * 360.0 / $NUM_PLANES ]
	for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
		set T [expr $T_per * $i / $SATS_PER_PLANE ]
		satellites LOAD $a $e $inc $Omega $omega $T "Spaceway-NGSO ($j, $i)"
	}
}
satellites GV_END
