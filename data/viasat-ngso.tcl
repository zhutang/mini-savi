#
# * ViaSat NGSO
# *
# * A Medium Earth Orbit (MEO) system. 
# *
# * Described in ViaSat's filing to the US FCC: Petition for
# * Declatory Ruling, SAT-LOI-20161115-00120, 15 November 2016.
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://savi.sourceforge.net/
#
# $Id: viasat-ngso.tcl,v 1.1 2016/11/19 07:15:52 lloydwood Exp $

set SATS_PER_PLANE 8
set NUM_PLANES 3
set INTERPLANE_SPACING 120
set PHASING 0.0

# setup orbital elements
set a [expr 8200.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 87.0
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# in Appendix A.
set coverage_angle 25.0

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
	   satellites LOAD $a $e $inc $Omega $omega $T "ViaSat NGSO ($j, $i)"
	}
}
satellites GV_END
