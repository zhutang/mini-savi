#
# * Deligo
# *
# * A satellite constellation designed at the University of Surrey.
# *
# * This "provides 100% single satellite visibility between +/-74deg
# * latitudes and 100% dual diversity between +/-62deg latitudes
# * at a minimum elevation angle of 14deg" with a repetitive ground
# * track. Not built.
# *
# * Described in: 
# *  C. Meenan, A. Sammut, R. Tafazolli and B. G. Evans, Deligo:
# * Repetitive ground track 100% dual-diversity LEO satellite
# * constellation for S-PCN, IEE Electronics Letters, vol. 13
# * no. 17, 17 August 1995, pp. 1407-1408.
#
# $Id: deligo.tcl,v 1.2 2011/05/15 16:14:25 lloydwood Exp $

set SATS_PER_PLANE 8
set NUM_PLANES 8

# setup orbital elements
set a [expr 1626.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 54.0
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

set coverage_angle 14.0

# Plane offset is really a function of harmonic factor in Ballard constellations.
# (Rosette Constellations of Earth Satellites, A. H. Ballard, TRW,
#  IEEE Transactions on Aerospace and Electronic Systems, Vol 16 No 5, Sep. 1980)
# 360 / 8 / 8 = 360/64 = 5.625 degrees.
# quoted 1/8 harmonic factor is that.

satellites GV_BEGIN
for {set j 0} {$j < $NUM_PLANES} {incr j} {
	set Omega [expr $j * 360.0 / $NUM_PLANES]
	for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
		set T [expr $T_per * ( double($i) / $SATS_PER_PLANE - 5.625/360.0*$j) ]
		set n [satellites LOAD $a $e $inc $Omega $omega $T "Deligo ($j, $i)"]
		if {$i > 0} {satellites ORBIT_SET $n 0}
	}
}
satellites GV_END
