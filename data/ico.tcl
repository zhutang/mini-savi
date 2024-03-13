#
# * ICO
# *
# * ICO is Intermediate Circular Orbit, another term for Medium Earth
# * Orbit, or MEO. ICO began at Inmarsat, where it was known as
# * Inmarsat-P or Project 21, as a response to the developments of
# * Iridium and Globalstar to provide voice telephony and low-rate data.
# * Contract work was done with TRW; this also led to the Odyssey system,
# * which was cancelled in December 1997 in favour of ICO.
# *
# * GSM terrestrial mobile cellular telephony became widespread,
# * and the construction, launch and bankruptcies of the competing
# * Iridium and Globalstar efforts indicated a shift in the market.
# *
# * ICO filed for US Chapter 11 bankruptcy protection in August 1999.
# * ICO then emerged from bankruptcy protection in May 2000, after its
# * first launch lost a satellite in March 2000. The second launch in
# * June 2001 succeeded. None of the other constructed MEO ICO satellites
# * have yet been launched.
# *
# * (ICO Global's G1 geostationary satellite was launched in April 2008, and
# * was claimed to be the largest geostationary satellite yet launched.
# * This is intended to provide "ICO mim" (mobile interactive media)
# * services, including mobile video, over the United States.)

# Zero phasing between planes, since odd number of sats 180deg opposed
# means that sats cross the other plane halfway between sats in that plane,
# which is ideal for coverage.
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: ico.tcl,v 1.11 2016/01/09 23:04:34 lloydwood Exp $

set SATS_PER_PLANE 5
set NUM_PLANES 2

# setup orbital elements
# altitude was 10,355km until recently.
# Noticed and confirmed change to 10,390km in October 1998.
set a [expr 10390.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 45.0
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# elevation angle of 10 degrees

set coverage_angle 10.0

satellites GV_BEGIN
for {set j 0} {$j < $NUM_PLANES} {incr j} {
	set Omega [expr $j * 360.0 / $NUM_PLANES ]
	for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
		set T [expr $T_per * $i / $SATS_PER_PLANE ]
		satellites LOAD $a $e $inc $Omega $omega $T "ICO ($j, $i)"
	}
}
satellites GV_END
