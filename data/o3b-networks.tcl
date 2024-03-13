#
# * NON-GEOSTATIONARY EQUATORIAL RING - OPERATIONAL SYSTEM
# *
# * O3b
# *
# * See http://www.o3bnetworks.com/
# *
# * Named for 'the other three billion people' without good network
# * service.
# *
# * Announced with much publicity in September 2008. Investment from
# * Google, SES, and others. First four satellites launched in June 2013,
# * with operational traffic from January 2014. Second four satellites
# * launched July 2014. Third four satellites launched December 2014.
# *
# Satellite altitude has been altered slightly over time. See:
#
# Sea Launch signs launch agreement with O3b Networks, press release,
# 23 September 2008.
# ("equatorial injection orbit of 7,825 kilometers above the earth")
#
# O3b signs agreement with Arianespace, press release, 17 March 2010.
# ("nearly 8,000 kilometers from the earth")

# http://www.o3bnetworks.com/AboutUs/faq.html checked 8 April 2011.
# "O3b is positioning its satellites in an orbit 8,063km from
# Earth.")

# Website currently says 8,062km altitude.
# http://www.o3bnetworks.com/o3b-advantage/our-technology
# checked 12 October 2014.

# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: o3b-networks.tcl,v 1.23 2017/01/02 11:30:45 lloydwood Exp $

# * Stated deployment of satellites at 8,062km altitude,
# * but only six of the first eight satellites launched were active,
# * due to power converter problems with the first four satellites.
# * Two of the faulty satellites are not in service, acting as
# * on-orbit spares for the other two. See:
# * 'Two O3b Satellites Taken Out of Service as a Precaution,'
# * Peter de Selding, Space News, 11 September 2014.
# *
# * After four further satellites were launched in December 2014,
# * it was reported in May 2016 that nine satellites are active. See:
# * 'SES to take control of O3b Networks,' Space Daily, 6 May 2016.
# *
# http://www.spacenews.com/article/satellite-telecom/41831world-satellite-business-week-two-o3b-satellites-taken-out-of-service
# http://www.spacedaily.com/resports/SES_to_take_control_of_O3b_Networks_999.html

# * O3b Networks filed with the US FCC in November 2016 to add satellites
# * in high-inclination 70 degree orbits to cover polar latitudes and complete
# * global coverage. Two planes of eight satellites each are planned.
# * Further satellites with new payloads and frequencies are added to share
# * the existing equatorial ring. These are simulated in the other script
# * (o3b-networks-full.tcl), which you can Load... in
# * to add to this existing constellation.

set NUM_SATS [expr 4 - 2 + 4 + 4 - 1]

# eight further satellites are planned for this ring.
# uncomment to add.
# set NUM_SATS [expr $NUM_SATS + 8]

# setup orbital elements
set a [expr 8062.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 0.0
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# We're told coverage between 45 degrees of latitude.
# Ka-band, so a relatively high mask angle can be expected.
# But mask must be low to cover up to 45 deg lat consistently.
# > 5 degrees elevation for 7.3m diameter gateway stations.
# > 15 degrees elevation for 2.4m diameter and larger antennas.
# > 20 degrees elevation for 1.8m diameter antennas.

set coverage_angle 15.0

satellites GV_BEGIN

set T [expr $T_per ]
for {set j 0} {$j < $NUM_SATS} {incr j} {
	set Omega [expr $j * 360.0 / $NUM_SATS]
	satellites LOAD $a $e $inc $Omega $omega $T "03b Networks-$j"
}
satellites GV_END
