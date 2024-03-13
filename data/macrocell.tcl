#
# * Macrocell
# *
# * Macrocell was a followup proposal from Iridium, intended to provide
# * S-band voice and data. Not built.
# *
# * See Iridium's FCC filing of 26 September 1997.
# * Orbital parameters provided in table 2, page A-7.
# * SaVi simulation can be verified against picture on page A-8.
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: macrocell.tcl,v 1.8 2015/08/31 01:29:33 lloydwood Exp $

set SATS_PER_PLANE 12
set NUM_PLANES 8


# setup orbital elements
set a [expr 852.95+$RADIUS_OF_EARTH]
set e 0.0013
set inc 98.8
set omega 270.0

set coverage_angle 15.0

# compute period of orbit
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]


satellites GV_BEGIN
for {set j 0} {$j < $NUM_PLANES} {incr j} {
	set Omega [expr $j * 23.5 ]
	if { $j % 2 == 0} {
		set plane_offset 0
	} else {
		set plane_offset [expr $T_per / $SATS_PER_PLANE / 2.0]
	}
	for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
	        set T [expr $T_per * $i / $SATS_PER_PLANE + $plane_offset]
		satellites LOAD $a $e $inc $Omega $omega $T "Macrocell ($j, $i)"
	}
}
satellites GV_END

