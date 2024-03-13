#
######################################################
#
#  SaVi by Lloyd Wood (lloydwood@users.sourceforge.net),
#          Patrick Worfolk (worfolk@alum.mit.edu) and
#          Robert Thurman.
#
#  Copyright (c) 1997 by The Geometry Center.
#  Also Copyright (c) 2017 by Lloyd Wood.
#
#  This file is part of SaVi.  SaVi is free software;
#  you can redistribute it and/or modify it only under
#  the terms given in the file COPYRIGHT which you should
#  have received along with this file.  SaVi may be
#  obtained from:
#  http://savi.sourceforge.net/
#  http://www.geom.uiuc.edu/locate/SaVi
#
######################################################
# From Mark Eichin, Cygnus Support.
# Souped up a bit by Stuart Levy <slevy@geom.umn.edu>
# http://www.cygnus.com/~eichin/random-tcl.html
# now try:
# http://www.mit.edu/afs/sipb/project/eichin/common/public_html/random-tcl.html
#
# $Id: random.tcl,v 1.18 2017/01/02 06:11:33 lloydwood Exp $

proc rand(raw) {} {
    global rand_seed
# per Knuth 3.6:
# 65277 mod 8 = 5 (since 65536 is a power of 2)
# c/m = .5-(1/6)\sqrt{3}
# c = 0.21132*m = 13849, and should be odd.
    set rand_seed [expr (65277*$rand_seed+13849)%65536]
    set rand_seed [expr ($rand_seed+65536)%65536]
    return $rand_seed
}

# [rand base] returns a random number in range 0..(base-1).
# Returns integer or floating-point according to the type of "base".
# With no parameter, [rand] returns floating-point value in range 0..1.
proc rand {{base 1.0}} {
    set rr [rand(raw)]
    return [expr ($rr*$base)/65536]
}

# Seed random generator.
proc srand {{seed ""}} {
    if {$seed == ""} {
	# Initial seed from process-id and access-time of UNIX current dir
	set seed [expr [pid] + [file atime .]]
    }
    if {$seed != int($seed)} {
	set seed [expr int(($seed-int($seed))*65536)]
    }
    global rand_seed
    set rand_seed [expr $seed%65536]
}

srand


# Produce an ordered list of N values spaced at uniform-randomly-distributed
# intervals over range 0..Max

proc randlist {N {Max 1.0}} {
  set sum 0
  for {set i 0} {$i < $N} {incr i} {
    set v [rand 1.0]
    set sum [expr $sum+$v]
    lappend vals $sum
  }
  set scale [expr $Max/$sum]
  if {[string match {*[.eE]*} $Max]} {
    foreach v $vals {
	lappend result [expr $v*$scale]
    }
  } else {
    foreach v $vals {
	lappend result [expr int($v*$scale)]
    }
  }
  lrange $result 0 [expr $N-2]
}

# return a random RRGGBB color value
# This isn't perceptually weighted.
proc random_color {} {

    # set color_range [expr 256*256*256.0]
    # set random_col [expr int([rand $color_range])]
    # only seems to generate tasteful autumn shades

    set rr [rand 255]
    set gg [rand 255]
    set bb [rand 255]

    set random_col [expr int(65536*$rr + 256*$gg + $bb)]

    return $random_col

}
