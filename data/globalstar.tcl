#
# * OPERATIONAL SYSTEMS
# *
# * Globalstar
# *
# * This is the 'perfect' fully-deployed first-generation Globalstar
# * system, which provides voice and low-rate data services.
# *
# * Full constellation launched as of November 1999; notably lost
# * twelve satellites on a failed Zenit 2 launch in September 1998.
# * Service officially launched in October 1999.
# *
# * Designed for voice telephony and low-rate data. In the decade while
# * Globalstar was being designed and built for Qualcomm, GSM terrestrial
# * mobile cellular telephony became widespread, removing the intended
# * 'business traveller' market.
# * Filed for US Chapter 11 bankruptcy protection
# * February 2002, and re-emerged in April 2004.
# *
# * Despite the name, this does not offer global coverage, and is
# * not a Walker polar star constellation. The geometry is that
# * of a Walker delta, or a Ballard rosette.
# * Does not use intersatellite links; connections are only completed
# * when a ground gateway station shares a footprint with a terminal.
# * Service and coverage areas are limited by the number of available
# * ground stations; there is no service over most oceans, because
# * there is no gateway station to complete and connect the call.
# *
# * Onboard S-band transponders amplifiers failing
# * and preventing voice service on a number of Globalstar satellites
# * are another limitation on service availability.
# *
# * A replacement Globalstar-2 constellation was completely launched
# * and in service as of September 2013, with satellites built by
# * Thales Alenia Space in France. Ocean coverage remains limited.
# *
# * See also http://www.globalstar.com/
#
# $Id: globalstar.tcl,v 1.14 2016/08/16 02:02:59 lloydwood Exp $

set SATS_PER_PLANE 6
set NUM_PLANES 8

# setup orbital elements
set a [expr 1406.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 52.0
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

set coverage_angle 10.0

satellites GV_BEGIN
for {set j 0} {$j < $NUM_PLANES} {incr j} {
	set Omega [expr $j * 360.0 / $NUM_PLANES]
	for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
		set T [expr $T_per * ( double($i) / $SATS_PER_PLANE - 7.5/360.0*$j) ]
		set n [satellites LOAD $a $e $inc $Omega $omega $T "Globalstar ($j, $i)"]
		if {$i > 0} {satellites ORBIT_SET $n 0}
	}
}
satellites GV_END
