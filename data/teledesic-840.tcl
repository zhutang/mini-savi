#
# * PROPOSED LARGE BROADBAND CONSTELLATIONS - FAT PIPE DREAMS
# *
# * Teledesic (original Calling 840-active-satellite design)
# *
# * Uses intersatellite links, which are not yet simulated here.
# *
# * Announced with much publicity in 1994. With 840 active satellites,
# * and 84 on-orbit spares for 924 satellites in orbit, this was
# * the largest commercial proposal and the height of dot-com decade
# * optimism.
# *
# * The constellation is a Walker polar star; there is a 2-degree hole
# * in coverage at the poles.
# *
# * Redesigned in 1997 to a reduced 288-active satellite system.
# *
# * The simulation shows optimum ABABA spacing between co-rotating
# * planes. In reality, maintaining phasing between planes for all
# * these satellites is recognised to be impossible, making phasing
# * offsets random.
# *
# * See:
# * M. Sturza, Architecture of the Teledesic satellite system,
# * Proceedings of the International Mobile Satellite Conference '95,
# * Ottawa, pp. 214-218.
# *
# * M. H. Lawrence, D. P. Patterson, J. R Stuart and E. F. Tuck,
# * The Calling network: a global wireless communications system,
# * International Journal of Satellite Communications. Vol. 12, No. 1,
# * Jan-Feb 1994, pp.45-61.
# *
# * Satellite communication system, Teledesic LLC,
# * US Patent 6157621, granted November 2000.
#
# $Id: teledesic-840.tcl,v 1.8 2011/04/27 09:02:38 lloydwood Exp $

set SATS_PER_PLANE 40
set NUM_PLANES 21
set INTERPLANE_SPACING 9.5

# setup orbital elements
set a [expr 750.0+$RADIUS_OF_EARTH]
set e 0.00118
set inc 98.7
set omega 0.0

# Ka-band and 40 degrees mask elevation.
set coverage_angle 40.0

# compute period of orbit
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]

satellites GV_BEGIN
for {set j 0} {$j < $NUM_PLANES} {incr j} {

	# space planes evenly so that intersection with the
	# equatorial plane are separated by INTERPLANE_SPACING.
	set Omega [expr $j * $INTERPLANE_SPACING]

	# put planes at different heights
	set a_plane [expr $a+0.5*$j]

	for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
		set T [expr $T_per * ($i + $j/double($NUM_PLANES)) / \
			$SATS_PER_PLANE ]
		set n [satellites LOAD $a_plane $e $inc $Omega $omega $T "Teledesic-1994 ($i, $j)"]
		# only display one orbit in each plane
		if {$i > 0} {satellites ORBIT_SET $n 0}
	}
}
satellites GV_END
