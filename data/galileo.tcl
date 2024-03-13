#
# * NAVIGATION SYSTEMS
# *
# * Galileo navigation constellation
# *
# * Proposed as a competitor to GPS.
# *
# * Not yet operational, other than the Galileo In-Orbit Validation
# * Element (GIOVE) demonstrator satellites, which do not form part of
# * this constellation.
# *
# * Best viewed with a large number of colours (14+) to show full
# * diversity.
#
# http://europa.eu.int/comm/dgs/energy_transport/galileo/
# http://europa.eu.int/comm/dgs/energy_transport/galileo/doc/galileo_hld_v3_23_09_02.pdf
# Galileo Mission High Level Definition, version 3, 23 Sep 2002.
# see section 4.1.1
#
# http://www.aoe.vt.edu/~cdhall/Space/archives/000296.html
# The GALILEO constellation will comprise 27 satellites (and four
# spares) in circular orbits at 23,616 km altitude, in 56 degree
# inclined orbits, with nine operational satellites equally spaced
# in each of three equally spaced orbital planes. This configuration
# is known as a Walker 27/3/1 constellation, where "27" is the number
# of satellites, "3" is the number of planes, and "1" is a parameter
# defining the phasing or mean anomaly difference between satellites
# in adjacent planes. The constellation has the notable property that
# for any minimum elevation angle, the number of visible GALILEO
# satellites is equal to the number of visible GPS satellites
# plus one.
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: galileo.tcl,v 1.12 2015/08/31 01:29:33 lloydwood Exp $

# We are excluding all spares.
set SATS_PER_PLANE 9
set NUM_PLANES 3

# setup orbital elements
set a 29994.0
set e 0.0
set inc 56.0
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

upvar #0 NUM_COLORS NUM_COLORS

if {$NUM_COLORS < 12} {
  # 12 satellites is maximum diversity seen in fisheye for Galileo.
  puts stderr "\nSaVi: Coverage view of Galileo constellation benefits from a large number of colors."
}

# Walker /1 for deltas means Ballard first harmonic.
# each plane is offset by 360/27*1 = 13.33 degrees.
# Third plane is 13.33 degrees below the first plane.

# table 8 of Mission HLD says:
# receiver mask   visible Galileo    visible GPS
#  5              13                 12
# 10              11                 10
# 15               9                  8
# ...but is unclear if this includes transmitting spares.

# set mask to 0
set coverage_angle 0.0

set phase_offset [expr $T_per / $NUM_PLANES / $SATS_PER_PLANE]

satellites GV_BEGIN

for {set j 0} {$j < $NUM_PLANES} {incr j} {

   set Omega [ expr $j * 360.0 / $NUM_PLANES ]

   for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {

       set T [expr ($T_per * $i / $SATS_PER_PLANE - $phase_offset*$j)]
       satellites LOAD $a $e $inc $Omega $omega $T "Galileo ($j, $i)"
   }
}

satellites GV_END
