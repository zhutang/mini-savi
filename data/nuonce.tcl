#
# * NUONCE
# *
# * NonUniform Optimal Network Communications Engine
# *
# * See:
# *  M. W. Lo and P. Estabrook, The NUONCE Engine for LEO Networks,
# * NASA Jet Propulsion Lab, Proceedings of the Fourth International
# * Satellite Mobile Conference, 1995, pp. 193-197.
# *
# * Martin and Polly are thanked in SaVi's README.
# *
# * See also:
# *  M. W. Lo, Satellite constellation design, Computing in Science and
# * Engineering, Jan-Feb 1999, pp. 58-67.
#
# $Id: nuonce.tcl,v 1.7 2011/03/12 16:05:22 lloydwood Exp $

# setup orbital elements
set a [expr 800+$RADIUS_OF_EARTH]
set e 0
set inc 98.7
set omega 0.0

# Uncertain of minimum elevation angle; set to zero.
set coverage_angle 0.0

set Omegas {15 30 45 90 135 150 165}
set n_sats { 6  6  6  4   6   6   6}

# compute period of orbit
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]


satellites GV_BEGIN
set j 0
foreach Omega $Omegas {
	set ns [lindex $n_sats $j]
	for {set i 0} {$i < $ns} {incr i} {
		set T [expr $T_per * $i / $ns]
		satellites LOAD $a $e $inc $Omega $omega $T "NUONCE ($j, $i)"
	}
	incr j
}
satellites GV_END
