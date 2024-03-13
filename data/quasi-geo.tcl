#
# * THEORETICAL CONSTELLATIONS
# *
# * Quasi-geostationary constellation
# *
# * A three-satellite inclined geosynchronous system.
# * This was researched for Japan. It is intended to provide coverage
# * of higher latitudes while still allowing the high mask elevation
# * needed for high-frequency use.
# *
# * This simulation is intended to match what is described in:
# *  T. Taleb, U. Dharmaratna, N. Kato, and Y. Nemoto, A Geographical
# * Location Based Satellite Selection Scheme for a Novel Constellation
# * Composed of Quasi-Geostationary Satellites, Proceedings of IEEE
# * International Conference on Communications, ICC 2005, Seoul, Korea,
# * May 2005.
# http://dx.doi.org/10.1109/ICC.2005.1494588
# * 
# * See also:
# *  T. Taleb, A. Jamalipour, N. Kato, and Y. Nemoto, A Theatre in the
# * Sky: A Ubiquitous Broadband Multimedia-on-Demand Service over a
# * Novel Constellation Composed of Quasi-Geostationary Satellites,
# * International Journal of Satellite Communications and Networking,
# * Vol. 24, No. 3, May/June 2006. pp. 215-227.
# http://doi.wiley.com/10.1002/sat.829
# *  T. Taleb, Internetworking over Next-Generation NGEO/IP-based
# * Satellite Communications Systems, PhD thesis, Tohoku University,
# * August 2005. See chapter 6.
# http://www.nemoto.ecei.tohoku.ac.jp/%7Etaleb/Library/phd_thesis.pdf
# *
# * Taleb gives a central longitude of 118 degrees for East Asia in
# * Fig 6.2 of his thesis. We use 140 degrees to also cover the
# * Australian and New Zealand landmasses, so that coverage is useful
# * in the southern hemisphere as well.
# *
# * Taleb repeats this three-satellite constellation six times for full
# * equatorial coverage. The script can be edited to show this.
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://savi.sourceforge.net/
#
# $Id: quasi-geo.tcl,v 1.11 2011/04/27 09:02:38 lloydwood Exp $


# Draw single plane over Japan for illustrative purposes.
# To see global coverage as described in Taleb's work, set this to 6.
set NUM_PLANES 1

set desired_longitude 140

set SATS_PER_PLANE 3

# setup orbital elements
set a [expr 35786.1+$RADIUS_OF_EARTH]
set e 0.0
set inc 45.0
set omega 0.0
set Omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# Presume high-frequency Ka-band use
set coverage_angle 40.0

set offset_between_planes [expr 360.0 / $NUM_PLANES ]

satellites GV_BEGIN

for {set i 0} {$i < $NUM_PLANES} {incr i} {

    set sat_longitude [expr $desired_longitude + $i * $offset_between_planes]

    for {set j 0} {$j < $SATS_PER_PLANE} {incr j} {
	# SaVi handles wraparound beyond +/-180
	set Omega [ expr 360.0/$SATS_PER_PLANE * $j + $sat_longitude]
	set T [expr $T_per / 3 * $j]
	satellites LOAD $a $e $inc $Omega $omega $T "Quasi-GEO-$j"
    }

}

satellites GV_END
