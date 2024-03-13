#
# * OPERATIONAL SYSTEMS
# *
# * Orbcomm - idealised full initial system.
# *
# * Orbcomm does not provide real-time services. Coverage gaps are not
# * considered a problem for terminals that store their messages and
# * forward them to a ground station when coverage is available.
# *
# * Earlier satellites are in different orbits, and do not coordinate
# * well with the main constellation.
# *
# * Filed for US Chapter 11 bankruptcy protection in 15 September 2000
# * and later re-emerged under the same ownership.
# * Does not use intersatellite links; connections are only completed
# * when a ground gateway station shares a footprint with a terminal.
# *
# * See also http://www.orbcomm.com/.
#
# From information supplied by Jim Franconeri
# (franconeri.jim@orbcomm.com)
#
# fourth Pegasus launch on 4 December 1999 was into an inclined
# orbit of 45 degrees, instead of the previously expected equatorial
# orbit. The RAANs are currently 0/120/240/20 (December 1999) but
# will be transitioning to the 0/90/180/270 you'd expect, shown here.
# Relative phasing between planes is not controlled and will drift
# over time; the phasing here is not optimal.
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: orbcomm.tcl,v 1.10 2015/08/31 01:29:33 lloydwood Exp $


# for main rosette constellation only
set SATS_PER_PLANE 8
set NUM_PLANES 4


# setup orbital elements

# we have some earlier satellites at different altitudes and inclinations
# to the main constellation rosette, so their coverage will not
# coordinate well with the coverage of the main constellation;
# you'll see multiple overlaps occurring, as well as varying
# coverage gaps. Since Orbcomm does not provide realtime service,
# coverage gaps are less important than for e.g. voice systems.
# since these first launches were to different altitudes, they will
# precess differently.

# no orbital eccentricity
set e 0.0

# altitudes
set a1 7115.14
set a2 7204.14
set a3 7198.14

# inclinations
set inc1 70
set inc2 108
set inc3 45

set omega 0.0

# mask elevation angle of 5 degrees
set coverage_angle 5.0

satellites GV_BEGIN

# first two satellites
set Omega 0
set T_per [expr 2 * $PI * pow($a1,1.5) / sqrt($MU)]

for {set i 0} {$i < 2} {incr i} {
   set T [expr ($T_per * $i / 2) ]
   satellites LOAD $a1 $e $inc1 $Omega $omega $T "Orbcomm-polar-$i"
}

# second two satellites
set Omega 0
set T_per [expr 2 * $PI * pow($a2,1.5) / sqrt($MU)]

for {set i 0} {$i < 2} {incr i} {
   set T [expr ($T_per * $i / 2) ]
   satellites LOAD $a2 $e $inc2 $Omega $omega $T "Orbcomm-retrograde-$i"
}


# three planes of eight satellites, and one of seven
set T_per [expr 2 * $PI * pow($a3,1.5) / sqrt($MU)]

for {set j 0} {$j < $NUM_PLANES} {incr j} {

   set Omega [ expr $j * 360.0 / $NUM_PLANES ]

   if {$j == 3} {
       set SATS_PER_PLANE 7 ;# launched 4 Dec 1999.
   }
   for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
       set T [expr ($T_per * $i / $SATS_PER_PLANE) ]
       satellites LOAD $a3 $e $inc3 $Omega $omega $T "Orbcomm ($j, $i)"
   }
}

satellites GV_END

#     Here is what the idealized Orbcomm constellation would look like:
#
#
#       Sat     A (km)   Inc (deg)  Ecc     ArgP(deg) Ra(deg)  M(deg)
#         1     7115.14    70.00  0.00000     0.00     0.00     0.00
#         2     7115.14    70.00  0.00000     0.00     0.00   180.00
#         3     7204.14   108.00  0.00000     0.00     0.00     0.00
#         4     7204.14   108.00  0.00000     0.00     0.00   180.00
#         5     7198.14    45.00  0.00000     0.00     0.00     0.00
#         6     7198.14    45.00  0.00000     0.00     0.00    45.00
#         7     7198.14    45.00  0.00000     0.00     0.00    90.00
#         8     7198.14    45.00  0.00000     0.00     0.00   135.00
#         9     7198.14    45.00  0.00000     0.00     0.00   180.00
#        10     7198.14    45.00  0.00000     0.00     0.00   225.00
#        11     7198.14    45.00  0.00000     0.00     0.00   270.00
#        12     7198.14    45.00  0.00000     0.00     0.00   315.00
#        13     7198.14    45.00  0.00000     0.00   120.00     0.00
#        14     7198.14    45.00  0.00000     0.00   120.00    45.00
#        15     7198.14    45.00  0.00000     0.00   120.00    90.00
#        16     7198.14    45.00  0.00000     0.00   120.00   135.00
#        17     7198.14    45.00  0.00000     0.00   120.00   180.00
#        18     7198.14    45.00  0.00000     0.00   120.00   225.00
#        19     7198.14    45.00  0.00000     0.00   120.00   270.00
#        10     7198.14    45.00  0.00000     0.00   120.00   315.00
#        21     7198.14    45.00  0.00000     0.00   240.00     0.00
#        22     7198.14    45.00  0.00000     0.00   240.00    45.00
#        23     7198.14    45.00  0.00000     0.00   240.00    90.00
#        24     7198.14    45.00  0.00000     0.00   240.00   135.00
#        25     7198.14    45.00  0.00000     0.00   240.00   180.00
#        26     7198.14    45.00  0.00000     0.00   240.00   225.00
#        27     7198.14    45.00  0.00000     0.00   240.00   270.00
#        28     7198.14    45.00  0.00000     0.00   240.00   315.00
#        29     7198.14    45.00  0.00000     0.00    20.00     0.00
#        30     7198.14    45.00  0.00000     0.00    20.00    51.43
#        31     7198.14    45.00  0.00000     0.00    20.00   102.86
#        32     7198.14    45.00  0.00000     0.00    20.00   154.29
#        33     7198.14    45.00  0.00000     0.00    20.00   205.71
#        34     7198.14    45.00  0.00000     0.00    20.00   257.14
#        35     7198.14    45.00  0.00000     0.00    20.00   308.57
#
# Operational constraints may change the final spacing somewhat.

