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
#
# utils.tcl
#
# $Id: utils.tcl,v 1.61 2017/01/02 06:11:33 lloydwood Exp $

proc window(raise) {window} {
    if {[winfo exists .$window]} {
	wm deiconify .$window
	raise .$window
	return 1
    }
    return 0
}

# Routines for helping to construct tk windows
#
# Construction routines:
#   build_Toplevel
#   build_Title
#   build_StdFrame
#   build_CmdFrame
#   build_Menubar
#   build_Menu
#   build_Buttonbar
#   build_DismissButtonbar
#   build_Optionslist
#   build_HOptionslist
#   build_Scrollingtext
#   build_Text
#   build_IPopupMenu
#   build_PopupMenu
#   build_LabelEntryColumns
#   build_Label
#   build_Message
#   build_Radiobutton
#
#
# Other routines:
#   build_Wname
#   update_Popupmenu
#   longest
#   loadfile_into_text

# New routines:
#   string_ends


# These procedures use the global variables COLOR and FONT in
# order to configure them.  In particular, they may reference:
#   COLOR(bg)
#   COLOR(abg)
#   COLOR(entry)
#   FONT(button)
#   FONT(label)
#   TITLE
#
# Other global variables used are:
#   main(title)  - for a string to appear in all titlebars made
#                  with build_Title


#
# string_ends {filename extension}
#
# returns true if filename ends with extension
#
proc string_ends {filename extension} {
    set file_length [string length "$filename"]
    set ext_length [string length "$extension"]
    set file_end [string range "$filename" \
	       [expr $file_length - $ext_length] $file_length]
    if {[string compare $file_end "$extension"] == 0} {
	return 1
    } else {
	return 0
    }
}

#
# build_Wname
#
# Converts name to a tk window name by placing a '.' at the beginning
#
# Returns: .name
#
proc build_Wname {name} {
    return .$name
}

#
# build_Toplevel
#
# Builds a new window using name (which must be unique).
# The name is passed to build_Wname to convert it to a Tk window name
#
# Returns: the tk window name
#
proc build_Toplevel {name} {

    set wname [build_Wname $name]
    if {![winfo exists $wname]} {
	toplevel $wname
    }
    return $wname
}


#
# build_Title
#
# Set the title of a toplevel window with the string "main(title) $title"
#
proc build_Title {name title} {
    global TITLE

    wm title $name "$TITLE: $title"
}

#
# build_StdFrame
#
# Builds a plain frame with specifed name in specified parent
# This widget is not packed.
#
# Returns: the tk frame widget name parent.name
#
proc build_StdFrame {parent name} {

    set fname $parent.$name
    if {![winfo exists $fname]} {
	frame $fname
    }
    return $fname
}

#
# build_CmdFrame
#
# Builds a raised frame with specifed name in specified parent
# to become an area of a window to put widgets
# This widget is not packed
#
# Returns: the tk frame widget name parent.name
#
proc build_CmdFrame {parent name} {

    set fname $parent.$name
    if {![winfo exists $fname]} {
	frame $fname -relief raised -bd 1
    }
    return $fname
}


#
# build_Menubar
#
# Usage:
#       build_Menubar parent name { helpmenu stuff } { menu stuff } ...
#
# Builds a menubar as specified.  The first menu specification is
# always placed all the way to the right and is expected to be a help menu.
# The others are placed starting at the left.
#
proc build_Menubar {parent name helpmenu args} {
    global FONT buttons_menu

    # create menubar

    if {$buttons_menu == 1} {
        set mbarname [ build_CmdFrame $parent $name ]
    } else {
        set mbarname $parent.$name
        menu $mbarname -tearoff 0
    }

    # menubars are not currently rebuilt!
    if {[winfo exists $mbarname.b0]} {
	puts stderr "\nbuild_Menubar: NOT rebuilding $mbarname"
	return
    }

    if {$buttons_menu == 1} {
        # put menubar at top of each windowpane
        # add help menubutton on right side

        # hack to show all text cut short by popup indicator
        set helpname [lindex $helpmenu 0]
        set helpname "$helpname "
        if {[llength $helpmenu] > 1} {
            menubutton $mbarname.help -text $helpname \
	                     -menu $mbarname.help.menu -font $FONT(button)
            eval build_Menu $mbarname.help [lrange $helpmenu 1 end]
	    pack $mbarname.help -side right
        }

        #add other menubuttons on left side
        set i 0 ; set menubuttons {}
        foreach arg $args {
	     if {[llength $arg] > 0} {
                # hack to show all text cut short by popup indicator
                # two spaces for long words
                set menuname [lindex $arg 0]
                set menuname "$menuname  "

                menubutton $mbarname.b$i -text $menuname \
		           -menu $mbarname.b$i.menu -font $FONT(button)
	        lappend menubuttons $mbarname.b$i
	        if {[llength $arg] > 1} {
                    eval build_Menu $mbarname.b$i [lrange $arg 1 end]
                }
	        incr i
	     }
        }
        if {[llength $menubuttons] > 0} {
	          eval pack $menubuttons -side left
        }

        # display menubar across the top of the panel
        pack $mbarname -side top -fill x

        return $mbarname
    }

    #add menus (except help) on left side of menubar area
    set i 0
    foreach arg $args {
	if {[llength $arg] > 0} {
	    $mbarname add cascade -menu $mbarname.b$i -label [lindex $arg 0] \
		-font $FONT(button)
	    if {[llength $arg] > 1} {
		eval build_Menu $mbarname.b$i [lrange $arg 1 end]
	    }
	    incr i
	}
    }

    # add help menu on right side
    if {[llength $helpmenu] > 1} {
	$mbarname add cascade -menu $mbarname.helpx -label [lindex $helpmenu 0] \
	    -font $FONT(button)
	eval build_Menu $mbarname.helpx [lrange $helpmenu 1 end]
    }

    # display menubar across the top of the panel
    $parent configure -menu $mbarname

    return $mbarname
}

#
# build_Menu
#
# Usage:
#	build_Menu mname {"Load..." load_open} {} {"Exit" tk_exit} ...
#
# Builds a menu with an arbitrary number of entries.
# Empty entries result in menu separators.
#
proc build_Menu {parent args} {
    global FONT buttons_menu modifier modifier_indicator

    # needed for Mac freeze workaround, which does not affect -redrawn-menus
    global buttons_menu

    # create menu
    if {$buttons_menu == 1} {
        set mname $parent.menu
        menu $mname -font $FONT(label)
    } else {
        set mname $parent
        menu $mname
    }

    set parent_window [string range $parent 0 [expr [string wordend $parent 1]-1]]

    # Do we disable on Mac to work around Mac menu+accelerator then
    # window draw freeze bug? This problem affects 10.6 with the default
    # supplied Tcl/Tk 8.5.7, when accelerator keys are used with Mac
    # menus.
    set disable_mac [expr ([string compare $modifier "Command"] == 0) && !$buttons_menu]
    # Tcl 8.5.9 under 10.7.2 or ActiveTcl 8.6b1.2 don't need this,
    # so the following line can be uncommented.
    # set disable_mac 0

    # add commands to menu
    foreach arg $args {
	if {[llength $arg] > 1} {
	    set command [lindex $arg 1]
	    set key [lindex $arg 2]
	    if {(!$disable_mac)&&([string length $key] > 0)} {
		set accelerator $modifier_indicator-[lindex $arg 2]
                set upperkey [string toupper $key]
                set lowerkey [string tolower $key]

                # We have to bind to .main, .coverage etc. Just . doesn't work
                bind $parent_window <$modifier-$upperkey> $command
                bind $parent_window <$modifier-$lowerkey> $command
	    } else {
	        set accelerator ""
	    }
	    $mname add command -label [lindex $arg 0] -command $command -accelerator $accelerator
	} else {
	    $mname add separator
	}
    }

    return $mname
}

#
# build_Buttonbar
#
# Usage: build_Buttonbar parent name {"Button1" action} ...
#
proc build_Buttonbar {parent name args} {
    global FONT

    # create buttonbar
    set bbarname [ build_CmdFrame $parent $name ]

    # add buttons
    set i 0
    foreach arg $args {
	if {[llength $arg] > 1} {
	    if { [winfo exists $bbarname.b$i] == 0} {
		button $bbarname.b$i -font $FONT(button)
		pack $bbarname.b$i -side left -padx 2 -pady 2
	    }
	    set name [lindex $arg 0]
	    if {([string compare $name "Cancel"] == 0) ||
		([string compare $name "Restart"] == 0) ||
	        ([string compare $name "Dismiss"] == 0) ||
		([string compare $name "Dismiss window"] == 0)} {
		# pad to avoid drawing under grow box (Mac OS X)
		pack $bbarname.b$i -side right -padx 15
	    }
	    $bbarname.b$i configure -text [lindex $arg 0] \
		-command [lindex $arg 1]
	    incr i
	}
	while { [winfo exists $bbarname.b$i] == 1} {
	    destroy $bbarname.b$i
	    incr i
	}
    }

    pack $bbarname -fill x

    return $bbarname
}

#
# build_DismissButtonbar
#
# Builds a button bar with a dismiss button on the right-hand side
# and the rest lined up on the left.
#
# Usage: build_DismissButtonbar parent name "dismiss command" \
#            {"Button1" "button1 command"} ...
#
proc build_DismissButtonbar {parent name dismiss args} {
    global FONT

    # create buttonbar
    set bbarname [ build_CmdFrame $parent $name ]

    # add other buttons
    set i 0
    foreach arg $args {
	if {[llength $arg] > 1} {
	    if { [winfo exists $bbarname.b$i] == 0} {
		button $bbarname.b$i -font $FONT(button)
		pack $bbarname.b$i -side left -padx 2 -pady 2
	    }
	    $bbarname.b$i configure -text [lindex $arg 0] \
		-command [lindex $arg 1]
	    incr i
	}
	while { [winfo exists $bbarname.b$i] == 1} {
	    destroy $bbarname.b$i
	    incr i
	}
    }

    # add dismiss button on right side
    if {[winfo exists $bbarname.dismiss] == 0} {
	button $bbarname.dismiss -font $FONT(button)
	# pad to avoid drawing under grow box (Mac OS X)
	pack $bbarname.dismiss -in $bbarname -side right -anchor e \
	    -padx 15 -pady 2
    }
    $bbarname.dismiss configure -text "Dismiss window" -command $dismiss

    pack $bbarname -side bottom -fill x
}


#
# build_Optionslist
#
# Procedure to make a list of option items
#
# Usage: build_Optionslist parent name \
#            {"Switch 1" switch1} {"Switch 2" switch2} ...
#
# The variables switch1,2 are set to 0 or 1 according to whether
# the switch is set or not.
#
proc build_Optionslist {parent name args} {
    global FONT

    # create options list
    set olistname [ build_StdFrame $parent $name]

    # add options
    set i 0
    foreach arg $args {
	if {[llength $arg] > 1} {
	    if { [winfo exists $olistname.b$i] == 0} {
		checkbutton $olistname.b$i -anchor w -font $FONT(label)
		pack $olistname.b$i -side top -fill x -padx 0.1c -pady 0.1c
	    }
	    set name [lindex $arg 0]
	    $olistname.b$i configure -text " $name" \
		-variable [lindex $arg 1]
	    incr i
	}
	while {[winfo exists $olistname.b$i] == 1} {
	    destroy $olistname.b$i
	    incr i
	}
    }

    pack $olistname -pady 0.1c -anchor w
}

#
# build_HOptionslist
#
# Procedure to make a horizontal list of option items
#
# Usage: build_HOptionslist parent name cmd \
#            {"Switch 1" switch1} {"Switch 2" switch2} ...
#
# The variables switch1,2 are set to 0 or 1 according to whether
# the switch is set or not, and then the cmd is executed.
#
proc build_HOptionslist {parent name cmd args} {
    global FONT

    # create options list
    set olistname [ build_StdFrame $parent $name]

    # add options
    set i 0
    foreach arg $args {
	if {[llength $arg] > 1} {
	    if { [winfo exists $olistname.b$i] == 0} {
		checkbutton $olistname.b$i -anchor w -font $FONT(label) -command $cmd
		pack $olistname.b$i -side left -padx 0.1c -pady 0.1c
	    }
	    $olistname.b$i configure -text [lindex $arg 0] \
		-variable [lindex $arg 1]
	    incr i
	}
	while {[winfo exists $olistname.b$i] == 1} {
	    destroy $olistname.b$i
	    incr i
	}
    }

    pack $olistname -pady 0.1c -anchor w
}


#
# build_Scrollingtext
#
# Builds a widget for text and a scrollbar
#
proc build_Scrollingtext {parent name width height} {
    global COLOR FONT

    # create stext frame
    set stext [ build_StdFrame $parent $name ]

    # add text widget
    if {[winfo exists $stext.text] == 0} {
	text $stext.text -relief sunken -bd 2 \
	    -yscrollcommand "$stext.scroll set" \
	    -bg $COLOR(entry) -font $FONT(label) \
	    -width 1 -height 1
	$stext.text configure -width $width -height $height -wrap word
	pack $stext.text -side left -fill both -expand 1
    }
    # add scrollbar
    if {[winfo exists $stext.scroll] == 0} {
       scrollbar $stext.scroll -command "$stext.text yview"
       pack $stext.scroll -side right -fill y
    }
    pack $stext -expand 1 -fill both
    return $stext.text
}

#
# build_Text
#
# Builds a widget for text
#
proc build_Text {parent name width height} {
    global COLOR FONT

    # build text widget
    if {[winfo exists $parent.text] == 0} {
	text $parent.text -relief sunken -bd 2 \
	    -bg $COLOR(entry) -font $FONT(label) \
	    -width 1 -height 1
	$parent.text configure -width $width -height $height
	pack $parent.text -fill both -expand 1
    }
    return $parent.text
}

#
# update_PopupMenu
#
# Utility routine for build_PopupMenu and build_IPopupMenu
#
proc update_PopupMenu {name str cmd args} {

    # $name configure -text "$str  ~"
    $name configure -text "$str"
    foreach arg $args {
	uplevel #0 $arg
    }
    eval $cmd
}

#
# build_IPopupMenu
#
# Constructs a popup menu
#
proc build_IPopupMenu { parent name label ivar command items } {
    global FONT

    set wname [build_StdFrame $parent $name]

    # make label
    if {![winfo exists $wname.label]} {
	label $wname.label -anchor w -font $FONT(label)
    }
    $wname.label configure -text $label

    # if no items then do not create menubar!
    if {![llength $items]} {
	if {[winfo exists $wname.mb]} {destroy $wname.mb}
	pack $wname.label -side left -padx 0.1c
	pack $wname -fill x -pady 0.1c
	return $wname
    }

    # make menubutton
    if {![winfo exists $wname.mb]} {
	menubutton $wname.mb -menu $wname.mb.m -anchor n -relief raised \
	    -font $FONT(label) \
	    -indicatoron 1
    }
    upvar #0 $ivar iv
    set w [expr [longest $items] + 4]
    $wname.mb configure -text "[lindex $items $iv]" -width $w

    # make menu
    if {[winfo exists $wname.mb.m]} {
	destroy $wname.mb.m
    }
    menu $wname.mb.m -font $FONT(label)
    set i 0
    foreach item $items {
	$wname.mb.m add command -label $item -command \
	    [list update_PopupMenu $wname.mb $item $command [list set $ivar $i]]
	incr i
    }

    pack $wname.label $wname.mb -side left -padx 0.1c
    pack $wname -fill x -pady 0.1c

    return $wname
}

#
# build_PopupMenu
#
# Constructs a popup menu
#
# Usage: build_PopupMenu parent name "Algorithm:" \
#            alg_var alg_proc {"Alg 0" "Alg 1" "Alg 2" ...}
# where alg_var is the variable which gives the index of the selected
# algorithm and alg_proc gets called whenever the algorithm is changed.
#
proc build_PopupMenu { parent name label ivar command items } {
    global FONT

    set wname [build_StdFrame $parent $name]

    # make label
    if {![winfo exists $wname.label]} {
	label $wname.label -anchor w -font $FONT(label)
    }
    $wname.label configure -text $label

    # if no items then do not create menubar!
    if {![llength $items]} {
	if {[winfo exists $wname.mb]} {destroy $wname.mb}
	pack $wname.label -side left -padx 0.1c
	pack $wname -fill x -pady 0.1c
	return $wname
    }

    # make menubutton
    if {![winfo exists $wname.mb]} {
	menubutton $wname.mb -menu $wname.mb.m -anchor n -relief raised \
	    -font $FONT(label) \
            -indicatoron 1
    }
    upvar #0 $ivar iv
    set w [expr [longest $items] + 4]
    $wname.mb configure -text "[lindex $items $iv]" -width $w

    # make menu
    if {[winfo exists $wname.mb.m]} {
	destroy $wname.mb.m
    }
    menu $wname.mb.m -font $FONT(label)
    set i 0
    foreach item $items {
	$wname.mb.m add command -label $item -command \
	    [list update_PopupMenu $wname.mb $item $command \
		 [list set $ivar $i]]
	incr i
    }

    pack $wname.label $wname.mb -side left -padx 0.1c
    pack $wname -fill x -pady 0.1c

    return $wname
}

##########################################################################
#
# utility: n-column builder
# notes:   builds n columns, each column is one of
#          entry or label widget
#
# build_Toplevel zoo col ZOO
# build_LabelEntryColumns lentry zoo.col sel \
#	{label "title1" {l1 l2 l3 l4} } 		\
#	{entry "title2" {e1 e2 e3 e4} } 		\
#	{foo   "title3" {f1 f2 f3 f4} } 		\
#
##########################################################################

proc build_LabelEntryColumns {parent name args } {
  global COLOR FONT PLATFORM

    build_StdFrame $parent $name

#    set _tablist {}

    set cn 0
    foreach c $args {
	set _type [lindex $c 0]
	set _code c$cn
	set _head [lindex $c 1]
	set _list [lindex $c 2]

	set valid_type 1
	set pady 0
	set col_config {$obj_n configure -textvariable $l}
#	set col_tab {set valid_type}
	if { [string compare $_type "label"] == 0 } {
	    set col_entry {label $obj_n -anchor n -font $FONT(label)}
	    set pady 2
	} elseif { [string compare $_type "dlabel"] == 0 } {
	    set col_entry {label $obj_n -width 23 -relief sunken -anchor w -font $FONT(label) }
	    set pady 2
	} elseif { [string compare $_type "ilabel"] == 0 } {
	    set col_entry {label $obj_n -width 10 -relief sunken -anchor w -font $FONT(label) }
	    if {$PLATFORM == 2} {
	        set pady 2
            } elseif {$PLATFORM == 1} {
                set pady 4
            } else {
	        set pady 3
            }
	} elseif { [string compare $_type "text"] == 0 } {
	    set col_entry {label $obj_n -anchor e -font $FONT(label)}
	    if {$PLATFORM == 2} {
	        set pady 2
            } elseif {$PLATFORM == 1} {
                set pady 4
            } else {
	        set pady 4
	    }
	    set col_config {$obj_n configure -text $l}
        } elseif { [string compare $_type "dentry"] == 0 } {
	    set col_entry {entry $obj_n -width 10 -relief sunken -bg $COLOR(entry) -font $FONT(label) }
#	    set col_tab { lappend _tablist $obj_n }
            if {$PLATFORM == 2} {
                set pady 4
            } elseif {$PLATFORM == 1} {
                set pady 1
            } else {
                set pady 2
            } 
        } elseif { [string compare $_type "ientry"] == 0 } {
	    set col_entry {entry $obj_n -width 10 -relief sunken -bg $COLOR(entry) -font $FONT(label)}
#	    set col_tab { lappend _tablist $obj_n }
            if {$PLATFORM == 2} {
                set pady 4
            } elseif {$PLATFORM == 1} {
                set pady 3
            } else {
	        set pady 2
            }
        } elseif { [string compare $_type "lentry"] == 0 } {
	    set col_entry {entry $obj_n -width 40 -relief sunken -bg $COLOR(entry) -font $FONT(label)}
#	    set col_tab { lappend _tablist $obj_n }
        } elseif { [string compare $_type "checkbox"] == 0 } {
	    set col_entry {checkbutton $obj_n -anchor w -variable $l -font $FONT(label)}
	    set col_config {$obj_n configure -text ""}
        } elseif { [string compare $_type "lcheckbox"] == 0 } {
	    set col_entry {checkbutton $obj_n -anchor w -variable $l -font $FONT(label)}
	    set col_config {$obj_n configure -text " Record ground tracks      and project forwards "}
	} elseif { [string compare $_type "slider"] == 0 } {
	    set col_entry {scale $obj_n -from 0 -to 100 -command $l \
			       -length 4c -showvalue false
	    }
	    if {$PLATFORM == 2} {
	        set pady 1
            } elseif {$PLATFORM == 1} {
                set pady 5
            } else {
                set pady 2
            }
	    set col_config {$obj_n configure -orient horizontal}
	} else {
	    # widget type not recognized
	    set valid_type 0
	    puts stderr "\nunknown widget name: $_type"
	}

	if {$valid_type} {
	    # create column and header
	    set obj $parent.$name.$_code
	    if { ![winfo exists $obj] } {
		frame $obj
		pack $obj -fill x -side left
		# -padx 2 -pady 1
	    }
	    if {[string length "$_head"]} {
		if { ![winfo exists $obj.head] } {
		    label $obj.head -anchor n -font $FONT(label)
		    pack $obj.head -side top -padx 2 -pady 1
		}
		$obj.head configure -text "$_head"
	    } else {
		if {[winfo exists $obj.head]} {
		    destroy $obj.head
		}
	    }

	    # add elements to column
	    set _n 0
	    foreach l $_list {
		set obj_n $obj.$_n
		if {![winfo exists $obj_n]} {
		    eval $col_entry
		    pack $obj_n -side top -padx 0.1c -pady $pady -fill x
		}
		eval $col_config
#		eval $col_tab
		incr _n
	    }
	    while { [winfo exists $obj.$_n] } {
		destroy $obj.$_n
		incr _n
	    }
	}
	# prepare for next column
	incr cn
    }

    while { [winfo exists $parent.$name.c$cn] } {
	destroy $parent.$name.c$cn
	incr cn
    }

#    if {[llength $_tablist] > 0} {
#	setTabbing $_tablist $name
#	focus [lindex $_tablist 0]
#    }

    pack $parent.$name -fill x -pady 0.1c

    return $parent.$name

}

#
# build_Label
#
# Usage: build_Label parent name message
#
proc build_Label {parent name message} {
    global FONT

    set fname [build_StdFrame $parent $name]
    if {[winfo exists $fname.label]} {
        $fname.label configure -text $message
    } else {
      label $fname.label -text $message -font $FONT(label)
      pack $fname.label -expand 1 -fill both
    }

    pack $fname -anchor w -padx 0 -pady 0
    return $fname
}

#
# build_options_menu
#
# Usage:
#	build_options_menu owner {"Load..." load_open} {} {"Exit" tk_exit} ...
#
# where the number of entries is arbitrary and empty entries result
# in separators.
#
proc build_options_menu {owner args} {
    global FONT buttons_menu

    if {$buttons_menu == 1} {
        set owner $owner.menu
    }

    # create menu
    if {[winfo exists $owner]} {
	destroy $owner
    }
    if {$buttons_menu == 1} {
        menu $owner -font $FONT(label)
    } else {
        menu $owner
    }

    # add commands to menu
    foreach arg $args {
	if {[llength $arg] == 2} {
	    $owner add checkbutton -label [lindex $arg 0] \
		-variable [lindex $arg 1]
	} else {
	    $owner add separator
	}
    }
}



# return the length of the longest string in a list
proc longest list {
    set len 0
    foreach l $list {
	set ll [string length $l]
	if {$ll > $len} {set len $ll}
    }
    return $len
}


#
# loads a file into the end of a text widget
#    returns 1 if successful, else 0
#
proc loadfile_into_text {textw filename} {
    set ok 0
    if {[file exists "$filename"]} {
	set f [open "$filename"]
	while {![eof $f]} {
	    $textw insert end [read $f 1000]
	}
	close $f
	set ok 1
    }
    return $ok
}
