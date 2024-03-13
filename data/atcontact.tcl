#
# * @contact
# *
# * Not built.
# *
# * See their FCC application, filed 22 December 1997.
# * Orbital characteristics are given in section 4.2.
#
# Was at http://www.atcontactcom.com/
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: atcontact.tcl,v 1.11 2015/08/31 01:29:33 lloydwood Exp $

# site says 16 operational satellites and four spares, one per plane.
set SATS_PER_PLANE 4
set NUM_PLANES 4

# setup orbital elements
set a [expr 10400.0+$RADIUS_OF_EARTH]
set e 0.001
set inc 45
set omega 0.0

set coverage_angle 20.0

set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# satellite phasing relative to adjacent plane of 22.5 deg.
# harmonic factor in Ballard constellations
# 360 / 4 / 4 = 360 / 16 = 22.5 degrees. 
set plane_offset [expr $T_per / $NUM_PLANES / $SATS_PER_PLANE]

satellites GV_BEGIN
for {set j 0} {$j < $NUM_PLANES} {incr j} {
	# 90 degrees between planes.
	set Omega [expr $j * 360.0 / $NUM_PLANES]

	for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
		# we use second harmonic as that appears to give best coverage
		# with dual or better coverage below 50 degrees of latitude.
		set T [expr $T_per * $i / $SATS_PER_PLANE - $plane_offset*2*$j]
		satellites LOAD $a $e $inc $Omega $omega $T "@contact ($j, $i)"
	}
}
satellites GV_END
