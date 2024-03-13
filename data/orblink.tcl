#
# * NON-GEOSTATIONARY EQUATORIAL RING - PROPOSAL
# *
# * Orblink
# *
# * Announced September 1997 by Orbital Sciences Corporation.
# * Not built.
# *
# * Intended use of intersatellite links, which are not yet
# * simulated here.
# *
# * Based on slides from:
# *  J. Bravman, J. Fedak and G. Giffin, Infrastructure on Demand -
# * Global Fiber-Optic Capacity from EHF Satellite Communications
# * Systems, Proceedings of the Fifth International Workshop on
# * Satellite Communications, June 1999.
# *
# * See also:
# *  K. Coale, Orbital launches into competition, Wired News,
# * 25 September 1997.
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: orblink.tcl,v 1.10 2015/08/31 01:29:33 lloydwood Exp $

set NUM_SATS 7

# setup orbital elements
set a [expr 9000+$RADIUS_OF_EARTH]
set e 0.0
set inc 0.0
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# Elevation angle of ten degrees in presentation
set coverage_angle 10.0

satellites GV_BEGIN

set T [expr $T_per ]
for {set j 0} {$j < $NUM_SATS} {incr j} {
	set Omega [expr $j * 360.0 / $NUM_SATS]
	satellites LOAD $a $e $inc $Omega $omega $T "Orblink-$j"
}
satellites GV_END
