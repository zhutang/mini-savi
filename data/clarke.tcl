#
# * THEORETICAL CONSTELLATIONS
# *
# * Clarke three-satellite geostationary constellation
# *
# * Clarke proposed three geostationary satellites spaced 120 deg at:
# *   30E - Africa and Europe
# *  150E - China and Oceana
# *   90W - the Americas
# *
# * In practice, Inmarsat's BGAN (Broadband Global Area Network) comes
# * closest to this - though it lacks the intersatellite links Clarke
# * mentions.
# *
# * There is slight drift over time even in the SaVi
# * model; geostationary satellites need to perform station-keeping
# * against perturbation anyway, and this drift is the only motion
# * visible.
# *
# * Arthur C. Clarke, 'Extra-Terrestrial Relays'
# * pp 305-308, Wireless World, October 1945.
#
# http://www.sciencemuseum.org.uk/on-line/clarke/ww1.asp
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: clarke.tcl,v 1.12 2015/08/31 01:29:33 lloydwood Exp $

# It's easiest to give each satellite its own RAAN, so we'll treat them
# as having separate planes even though they're not actually ascending.
set NUM_PLANES 3

# set location of one satellite; the others will be spaced from it
# east is positive.
set sat_longitude 150

# setup orbital elements
set a [expr 35786.1+$RADIUS_OF_EARTH]
set e 0.0
set inc 0.0
set omega 0.0
set Omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# for illustration purposes, but also a rough guide to practical use;
# although in e.g. Greenland, large dishes point almost to the horizon...
set coverage_angle 5.0

satellites GV_BEGIN

for {set i 0} {$i < $NUM_PLANES} {incr i} {
   # SaVi handles wraparound beyond +/-180
   set Omega [ expr 360.0/$NUM_PLANES * $i + $sat_longitude]
   set T 0
   satellites LOAD $a $e $inc $Omega $omega $T "Clarke-$i"
}

satellites GV_END
