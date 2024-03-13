#
# * PROPOSED LARGE BROADBAND CONSTELLATIONS - FAT PIPE DREAMS
# *
# * NeLS (Next-generation LEO System)
# *
# * Japanese proposal. A rosette with intersatellite links.
# *
# * Orbital details are given in an investigation of diversity issues:
# *
# * Hiroshi Tsunoda, Umith Dharmaratna, Nei Kato, Abbas Jamalipour
# * and Yoshiaki Nemoto, Network Controlled Handover for Improving
# * TCP Performance in LEO Satellite Networks, proceedings of
# * Globecom 2006 (see Table 1).
# *
# * Related diversity aspects and ns simulations discussed in:
# *
# * Umith Dharmaratna, Hiroshi Tsunoda, Nei Kato and Yoshiaki Nemoto,
# * A Proposal for Service Satellite Selection over LEO Satellite
# * Networks Using Geographical Location Information, Proceedings
# * of the 2003 Joint Conference on Satellite Communications (JC-SAT
# * 2003), National Museum of Emerging Science and Innovation, Tokyo,
# * 23-24 October 2003, pp. 181-193.
# http://register.itfind.or.kr/Report01/200401/KOSST/KOSST-0005/KOSST-0005.pdf
# *
# * This paper builds on the ideas in Lloyd Wood's PhD thesis (ch. 6)
# * and 2001 AIAA diversity paper.
#
# There are other related papers (not seen by Lloyd):
# http://ci.nii.ac.jp/naid/110003222474
# Umith Dharmaratna, Hiroshi Tsunoda, Nei Kato and Yoshiaki Nemoto
# A Satellite Selection Method for Walker Delta LEO Satellite Networks,
# IEICE transactions on communications E87-B(8), pp. 2124-2131, August
# 2004.
# Umith Dharmaratna, Hiroshi Tsunoda, Nei Kato, and Yoshiaki Nemoto,
# A Proposal for Service Satellite Selection over LEO Satellite
# Networks Using Geographical Location Information, IEICE Technical
# Report, SAT2003-123, pp. 7-12, 2003. 
#  
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: nels.tcl,v 1.11 2016/08/17 03:50:02 lloydwood Exp $

set SATS_PER_PLANE 12
set NUM_PLANES 10


# setup orbital elements
set a [expr 1200.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 55
set omega 0.0
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

# * Mask elevation angle said to be either 13 or 20 degrees; we would
# * choose the higher. Simulations suggest 18 degrees is needed for full
# * double surface coverage, so we use that.
set coverage_angle 18.0

# Plane offset is really a function of harmonic factor in Ballard constellations.
# (Rosette Constellations of Earth Satellites, A. H. Ballard, TRW,
#  IEEE Transactions on Aerospace and Electronic Systems, Vol 16 No 5, Sep. 1980)
# 360 / 12 / 10 = 360/120 = 3.0 degrees.
# From inspection we select harmonic factor of 9, which looks -almost-
# double mesh coverage at 20 degrees mask.
# Subtracting nine is the same as adding one.
#
set interplane_phasing [expr 360.0 / $NUM_PLANES / $SATS_PER_PLANE * 3 * 9] 

satellites GV_BEGIN

for {set j 0} {$j < $NUM_PLANES} {incr j} {

   set Omega [ expr $j * 360 / $NUM_PLANES ]

   set plane_offset [expr ($T_per / 360) * ($j * $interplane_phasing)  ]

   for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {

       set T [expr ($T_per * $i / $SATS_PER_PLANE) - $plane_offset ]
       satellites LOAD $a $e $inc $Omega $omega $T "NeLS ($j, $i)"
   }
}

satellites GV_END

# * This is not the definitive word on NeLS geometry. Also of interest:
# *
# * R. Suzuki and Y. Yasuda, Study on ISL network structure in LEO satellite
# * communication systems, Acta Astronautica, vol. 61, issues 7-8,
# * October 2007, pp. 648-658.
# http://dx.doi.org/10.1016/j.actaastro.2006.11.015
# * which recommends changes to that geometry.
