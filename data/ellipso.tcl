#
# * ELLIPTICAL SYSTEMS
# *
# * NON-GEOSTATIONARY EQUATORIAL-RING PROPOSALS
# *
# * Ellipso
# *
# * Combines highly-elliptical orbits useful near apogee with
# * an equatorial circular orbit.
# *
# * Designed by John E. Draim.
# * Combines a MEO equatorial ring (the Concordia constellation)
# * with an elliptical constellation covering the northern
# * hemisphere (the Borealis constellation). Ellipsat CEO David Castiel
# * was famously quoted in Wired as saying "Frankly, my business plan
# * can do without the people on Easter Island." (Joe Flower, Iridium,
# * Wired 1.05.) Its business plan focused on voice telephony and low-rate
# * data. GSM terrestrial mobile cellular telephony became widespread,
# * and the construction, launch and bankruptcies of the competing
# * Iridium and Globalstar efforts indicated a shift in the market.
# *
# * Not built. Ellipsat Inc. announced an alliance with ICO/Teledesic
# * in March 2001.
# *
# $Id: ellipso.tcl,v 1.11 2016/05/05 08:04:44 lloydwood Exp $

# to ensure overlapping coverage in the northern hemisphere
set coverage_angle 25

#
# Do ELLIPSO-BOREALIS
#

set SATS_PER_PLANE 5
set NUM_PLANES 2

set apogee_altitude 7846.0
set perigee_altitude 520.0

# * Setting minimum transmission altitude to 5,000 km works well
# * to turn off transmission away from apogee with some overlap.
# *
# * savi -min-transmit-altitude 5000

# setup orbital elements
set a [expr ($apogee_altitude+$perigee_altitude)/2+$RADIUS_OF_EARTH]
set e [expr ($apogee_altitude-$perigee_altitude)/(2*$a)]
set inc 116.565
set omega -90.0

# compute period of orbit
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]


satellites GV_BEGIN
for {set j 0} {$j < $NUM_PLANES} {incr j} {
	set Omega [expr $j * 360.0 / $NUM_PLANES]
	for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
		if { $j % 2 == 0} {
			set plane_offset 0
		} else {
			set plane_offset [expr $T_per / $SATS_PER_PLANE / 2.0]
		}
		set T [expr $T_per * $i / $SATS_PER_PLANE + $plane_offset]
		satellites LOAD $a $e $inc $Omega $omega $T "Ellipso-Borealis ($j, $i)"
	}
}
satellites GV_END



#
# Do ELLIPSO-CONCORDIA
#

# setup orbital elements
set a [expr 8040.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 0
set Omega 0.0
set omega 0.0

set SATS_PER_PLANE 6

# compute period of orbit
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

satellites GV_BEGIN
for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
	set T [expr $T_per*$i/$SATS_PER_PLANE]
	satellites LOAD $a $e $inc $Omega $omega $T "Ellipso-Concordia ($i)"
}
satellites GV_END
