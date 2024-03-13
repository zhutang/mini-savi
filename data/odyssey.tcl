#
# * Odyssey
# *
# * A Medium Earth Orbit (MEO) system for voice and low-rate data that was
# * developed by TRW. Odyssey was cancelled in December 1997 in favour of
# * the very similar ICO system.
#
# from Mitre Corporation report MTR93B0000157, February 1994, for ESA/ESTEC
# 'A reevaluation of selected mobile satellite communications systems:
#  Ellipso, Globalstar, IRIDIUM, Odyssey', Gaffney, Hulkower, Klein, Lam.
# Report is classed as 'controlled distribution', yet 'this report is
# based solely on open sources of information' (p2) so I can quote it.
# this report decribes Odyssey as 3 planes of 4 sats, 55 deg inclination,
# min 22 deg elevation angle.
# phasing is described as 'the phasing angle between planes one and two
# is 60 deg and between planes one and three is 30 deg'. (5.2.1, p87)
# 'this configuration provides continuous, near-global coverage by at least
# one satellite with a minimum elevation angle of 22 deg and by at least
# two satellites with a minimum elevation angle of 10 deg.'
#
# more recent info (e.g. Tor Wisloff's data) modifies that to 50 degs,
# 20 deg min elevation angle for one satellite; I assume the phasing is
# basically unchanged.
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: odyssey.tcl,v 1.9 2015/08/31 01:29:33 lloydwood Exp $

set SATS_PER_PLANE 4
set NUM_PLANES 3
set INTERPLANE_SPACING 120
set PHASING 0.0

# setup orbital elements
set a [expr 10354.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 50.0
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

set coverage_angle 20.0

satellites GV_BEGIN

for {set j 0} {$j < $NUM_PLANES} {incr j} {
	set Omega [expr $j * $INTERPLANE_SPACING ]
	if { $j == 1 } {
	   set PHASING 60
        }
	if { $j == 2 } {
	   set PHASING 30
        }
	set T_PHASE [expr $T_per * $PHASING / 360.0 ]
	for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
	   set T [expr $T_per * $i / $SATS_PER_PLANE + $T_PHASE ]
	   satellites LOAD $a $e $inc $Omega $omega $T "Odyssey ($j, $i)"
	}
}
satellites GV_END
