#
# * NAVIGATION SYSTEMS
# *
# * Global Positioning System (GPS)
# *
# * This shows nominal positions and spacing.
# *
# * Information provided by Peter Dana in 1996 while at the
# * University of Texas.
# *
# * Best viewed with a large number of colours (14+) to show full
# * diversity.
#
# Orbital summary now found at:
# http://www.colorado.edu/geography/gcraft/notes/gps/gps.html#SpaceSeg
# Nominal positions in plane taken from:
# http://www.colorado.edu/geography/gcraft/notes/gps/nomconst.html
#
# this script for SaVi by Lloyd Wood (lloydwood@users.sourceforge.net)
# http://personal.ee.surrey.ac.uk/Personal/L.Wood/software/SaVi/
#
# $Id: gps.tcl,v 1.10 2015/08/31 01:29:33 lloydwood Exp $

set SATS_PER_PLANE 4
set NUM_PLANES 6

# Uncertain of minimum elevation angle; set to zero.
set coverage_angle 0.0

# setup orbital elements
set a [expr 20200.0+$RADIUS_OF_EARTH]
set e 0.0
set inc 55.0
set omega 0.0

upvar #0 NUM_COLORS NUM_COLORS

if {$NUM_COLORS < 13} {
  # 13 satellites is maxium diversity seen in fisheye for GPS.
  puts stderr "\nSaVi: Coverage view of GPS constellation benefits from a large number of colors."
}

# GPS period is approximately 12 hours. Let's be a bit more exact...
set T_per [expr 2 * $PI * pow($a,1.5) / sqrt($MU)]


satellites GV_BEGIN

for {set j 0} {$j < $NUM_PLANES} {incr j} {
	set Omega [expr $j * 360.0 / $NUM_PLANES + 17 ]
	for {set i 0} {$i < $SATS_PER_PLANE} {incr i} {
#B1-4
	        if { $j == 0 } {
	           if { $i == 0 } {
	               set plane_offset 339.7
	           }
	           if { $i == 1 } {
	               set plane_offset 81.9
	           }
	           if { $i == 2 } {
	               set plane_offset 115.0
	           }
	     	   if { $i == 3 } {
	               set plane_offset 213.9
	           }
	        }
# C1-4
	     	if { $j == 1 } {
	           if { $i == 0 } {
	               set plane_offset 16.0
	           }
	           if { $i == 1 } {
	               set plane_offset 138.7
	           }
	           if { $i == 2 } {
	               set plane_offset 244.9
	           }
	     	   if { $i == 3 } {
	               set plane_offset 273.5
	           }
	        }
# D1-4
	     	if { $j == 2 } {
	           if { $i == 0 } {
	               set plane_offset 42.1
	           }
	           if { $i == 1 } {
	               set plane_offset 70.7
	           }
	           if { $i == 2 } {
	               set plane_offset 176.8
	           }
	     	   if { $i == 3 } {
	               set plane_offset 299.6
	           }
	        }
# E1-4
	       	if { $j == 3 } {
	           if { $i == 0 } {
	               set plane_offset 101.7
	           }
	           if { $i == 1 } {
	               set plane_offset 200.5
	           }
	           if { $i == 2 } {
	               set plane_offset 233.7
	           }
	     	   if { $i == 3 } {
	               set plane_offset 335.9
	           }
	        }
# F1-4
	   	if { $j == 4 } {
	           if { $i == 0 } {
	               set plane_offset 142.2
	           }
	           if { $i == 1 } {
	               set plane_offset 255.6
	           }
	           if { $i == 2 } {
	               set plane_offset 5.3
	           }
	     	   if { $i == 3 } {
	               set plane_offset 34.5
	           }
	        }
# A1-4
	       	if { $j == 5 } {
	           if { $i == 0 } {
	               set plane_offset 280.7
	           }
	           if { $i == 1 } {
	               set plane_offset 310.3
	           }
	           if { $i == 2 } {
	               set plane_offset 60.0
	           }
	     	   if { $i == 3 } {
	               set plane_offset 173.4
	           }
	        }

	        if { $plane_offset > 180 } {
	           set plane_offset [expr $plane_offset - 360]
	        }

		set T [expr $T_per * $plane_offset / 360 ]
		satellites LOAD $a $e $inc $Omega $omega $T "GPS ($j, $i)"
	}
}
satellites GV_END
