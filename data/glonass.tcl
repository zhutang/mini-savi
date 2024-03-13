#
# * NAVIGATION SYSTEMS
# *
# * Glonass navigation constellation
# *
# * The Russian alternative to GPS. This shows the perfect full
# * constellation; what's in orbit is only partial. Current status
# * information is available from:
# *  http://www.glonass-ianc.rsa.ru/ - click the British flag.
# *
# * Best viewed with a large number of colours (14+) to show full
# * diversity.
#
# http://www.spaceandtech.com/spacedata/constellations/glonass_consum.shtml
# has basic constellation information.
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: glonass.tcl,v 1.3 2015/08/31 01:29:33 lloydwood Exp $

# We are excluding all spares.
set SATS_PER_PLANE 8
set NUM_PLANES 3

# setup orbital elements
set a [expr 19100.0+$RADIUS_OF_EARTH] 
set e 0.0
set inc 64.8
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

upvar #0 NUM_COLORS NUM_COLORS

if {$NUM_COLORS < 11} {
  # 11 satellites is maximum diversity seen in fisheye for Glonass.
  puts stderr "\nSaVi: Coverage view of Glonass benefits from a large number of colors."
}

# set mask to 0
set coverage_angle 0.0

set phase_offset [expr $T_per / $NUM_PLANES / $SATS_PER_PLANE]

satellites GV_BEGIN

for {set j 0} {$j < $NUM_PLANES} {incr j} {

   set Omega [ expr $j * 360.0 / $NUM_PLANES ]

   for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {

       set T [expr ($T_per * $i / $SATS_PER_PLANE - $phase_offset*$j)]
       satellites LOAD $a $e $inc $Omega $omega $T "Glonass ($j, $i)"
   }
}

satellites GV_END
