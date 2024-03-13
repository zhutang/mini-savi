#
# * GS-2 (Globalstar proposal)
# *
# * Intended for voice and data in S-band.
# * Information from Globalstar's FCC filing of September 1997.
# * Orbital parameters provided on pages 7 and 8.
# * This SaVi simulation can be verified against picture on page 9.
# *
# * Uses intersatellite links, which are not yet simulated here.
# * The filing also mentions four geostationary satellites, not simulated.
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: gs2.tcl,v 1.11 2015/08/31 01:29:33 lloydwood Exp $

set SATS_PER_PLANE 8
set NUM_PLANES 8


# setup orbital elements
set a [expr 1420.0+$RADIUS_OF_EARTH]
set e 0.001
set inc 54.0
set omega 90.0

# minimum elevation mentioned in the Appendix for link budgets is 20 degrees,
# but that's insufficient for full coverage. So, we guess less - 14 is minimum.
set coverage_angle 14.0

# compute period of orbit
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# Plane offset is really a function of harmonic factor in Ballard constellations
# Rosette Constellations of Earth Satellites, A. H. Ballard, TRW,
#  IEEE Transactions on Aerospace and Electronic Systems, Vol 16 No 5, Sep. 1980
# 360 / 8 / 8 = 360/64 = 5.625 degrees.
# FCC application gives 39.375 degrees, which is what's left
# after subtracting from plane spacing of 45 degrees, or 7/8 harmonic.

satellites GV_BEGIN
for {set j 0} {$j < $NUM_PLANES} {incr j} {
	set Omega [expr $j * 360.0 / $NUM_PLANES ]

	# plane offset is 39.375, or 7/8 harmonic of 45
	set plane_offset [expr $T_per / 360 * 39.375 ]

	for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
	        set T [expr $T_per * $i / $SATS_PER_PLANE - $plane_offset * $j]
		satellites LOAD $a $e $inc $Omega $omega $T "GS-2 ($j, $i)"
	}
}

satellites GV_END
