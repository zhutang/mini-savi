#
# * ELLIPTICAL SYSTEMS
# *
# * Millimeter Wave Satellite proposal
# *
# * Uses elliptical orbits. Not built.
# *
# * Described in: A. H. Jackson and P. Christopher, A LEO Concept for
# * Millimeter Wave Satellite Communication, Proceedings of the Fourth
# * International Mobile Satellite Conference, Ottawa, 1995, pp. 185-192.
# *
# * A slightly different system is described in:
# * A. H Jackson and P. Christopher, Angle diversity for millimeter wave
# * satellite communications, Proceedings of the Fifth International
# * Conference on Satellite Systems for Mobile Communications and
# * Navigation, May 1996, pp. 51-54.
# http://dx.doi.org/10.1049/cp:19960407
#
# $Id: mm.tcl,v 1.9 2014/01/28 21:00:09 lloydwood Exp $

# number of satellites
set IMAX 30

# setup orbital elements
set a 8059.0
set e 0.1651396
set inc 63.435
set omega -45.0
set Omega 0.0
set T_per 0.0
set delta_T_per [expr -144.0/180.0*$PI/sqrt($MU/$a/$a/$a)]

# Uncertain of minimum elevation angle; set to zero.
set coverage_angle 0.0

satellites GV_BEGIN
for {set i 0} {$i < $IMAX} {incr i} {
		satellites LOAD $a $e $inc $Omega $omega $T_per "Millimeter-$i"
		set Omega [expr $Omega + 12]
		set T_per [expr $T_per + $delta_T_per]
}
satellites GV_END

