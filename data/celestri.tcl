#
# * PROPOSED LARGE BROADBAND CONSTELLATIONS - FAT PIPE DREAMS
# * 
# * Celestri
# *
# * This replaced Motorola's larger M-Star proposal. Not built.
# *
# * Uses intersatellite links, which are not yet simulated here.
# *
# * In May 1998, it was announced that Motorola and Teledesic would
# * be combining efforts on Teledesic, replacing the then Boeing
# * 288-active-satellite Teledesic design.
# *
# * information used from the Motorola FCC application, June 1997.
#
# Was available online in Adobe Acrobat pdf format from Motorola; see:
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/constellations/celestri/
# for an archived copy.
#
# Full details are in the scanned pdf of the section titled part two,
# on pp. 36-38, including 3D position plot and unprojected coverage
# map showing subsatellite points.
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: celestri.tcl,v 1.13 2015/08/31 01:29:33 lloydwood Exp $

set SATS_PER_PLANE 9
set NUM_PLANES 7


# setup orbital elements
set a [expr 1400.0+$RADIUS_OF_EARTH]
set e 0.0013
set inc 48
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# mask elevation angle is 16 degrees, according to page 42.
set coverage_angle 16.0

# Plane offset is really a function of harmonic factor in Ballard constellations.
# (Rosette Constellations of Earth Satellites, A. H. Ballard, TRW,
#  IEEE Transactions on Aerospace and Electronic Systems, Vol 16 No 5, Sep. 1980)
# 360 / 9 / 7 = 360/63 = 5.714 degrees.
# quoted 28.57 degrees is 5x that, or fifth harmonic factor.

set interplane_phasing [expr 360.0 / $NUM_PLANES / $SATS_PER_PLANE * 5]

satellites GV_BEGIN

for {set j 0} {$j < $NUM_PLANES} {incr j} {

   # FCC application gives 51.43 degrees between planes, but since it's a rosette
   # we can assume that they're just rounding off. More accurate:

   set Omega [ expr $j * 360 / $NUM_PLANES ]

   set plane_offset [expr ($T_per / 360) * ($j * $interplane_phasing)  ]

   for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {

       set T [expr ($T_per * $i / $SATS_PER_PLANE) - $plane_offset ]
       satellites LOAD $a $e $inc $Omega $omega $T "Celestri ($j, $i)"
   }
}

satellites GV_END
