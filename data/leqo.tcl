#
# * NON-GEOSTATIONARY EQUATORIAL RING - PROPOSAL
# *
# * LEqO
# *
# * A satellite constellation designed at the University of Surrey.
# *
# * A number of small satellites in a low equatorial orbit for
# * near-real-time communications for the tropics. Not built.
# *
# * Uses intersatellite links, which are not yet simulated here.
# *
# * W. Sun, M. Sweeting and A. da Silva Curiel,
# * LEO satellite constellation for regional communications,
# * Proceedings of IAF '96.
# http://web.archive.org/web/20040220030502/http://personal.ee.surrey.ac.uk/SSC/CSER/UOSAT/papers/iaf96/leqo/leqo.html
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: leqo.tcl,v 1.12 2015/08/31 01:29:33 lloydwood Exp $

set NUM_SATS 8

# setup orbital elements
set a [expr 968.2+$RADIUS_OF_EARTH]
set e 0.0
set inc 0.0
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# Elevation angle of five degrees for mobile users.
set coverage_angle 5.0

satellites GV_BEGIN

set T [expr $T_per ]
for {set j 0} {$j < $NUM_SATS} {incr j} {
	set Omega [expr $j * 360.0 / $NUM_SATS]
	satellites LOAD $a $e $inc $Omega $omega $T "LEqO-$j"
}
satellites GV_END
