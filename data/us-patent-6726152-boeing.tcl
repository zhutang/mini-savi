#
# * Boeing US patent 6726152
# *
# * See:
# *  US patent 6726152, Satellite communications system, Robert P. Higgins,
# * Boeing.
# http://www.patentstorm.us/patents/6726152.html
#
# References Lloyd's 'managing diversity' paper for no good reason.
# Phasing seems to have been chosen for a Walker delta; not the Ballard
# harmonic you might expect.
#
# this script for SaVi 1.2, by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/constellations/
#
# $Id: us-patent-6726152-boeing.tcl,v 1.5 2015/08/31 01:29:33 lloydwood Exp $

set SATS_PER_PLANE 5
set NUM_PLANES 4

# setup orbital elements
set a [expr 20182.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 57
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# elevation angle of 30 degrees
set coverage_angle 30.0

# offset between planes is 36 degrees - half of 72 degree phasing.
set offset 36

satellites GV_BEGIN
for {set j 0} {$j < $NUM_PLANES} {incr j} {
	set Omega [expr $j * 360.0 / $NUM_PLANES ]
	for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
		set T [expr $T_per * $i / $SATS_PER_PLANE + $T_per * $j * $offset/360]
		satellites LOAD $a $e $inc $Omega $omega $T "Boeing-Higgins ($j, $i)"
	}
}
satellites GV_END
