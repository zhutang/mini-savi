#
# Makefile - SaVi tcl directory. Run top-level Makefile instead.
#
# $Id: Makefile,v 1.21 2016/03/28 04:24:44 lloydwood Exp $

       SHELL = /bin/sh
          RM = /bin/rm -f

  TCL_SRCS = init.tcl main.tcl utils.tcl load.tcl save.tcl coverage_frames.tcl \
		coverage.tcl coverage_size.tcl edit.tcl fisheye.tcl params.tcl \
		geomview.tcl random.tcl about.tcl oe.tcl constellations.tcl \
	        no_frames.tcl snapshot.tcl rosette.tcl star.tcl empty.tcl \
                sunlight.tcl equator.tcl details.tcl helpfile.tcl \
		load_url_tle.tcl

     TCLSH = tclsh

# FreeBSD will require this uncommented
# TCLSH = tclsh8.5

# older Cygwin and Insight Tcl may require this uncommented
# TCLSH = cygtclsh80.exe

   MKINDEX = mkindex.tcl
OTHER_SRCS = $(TCL_SRCS) tclIndex

 CLEAN_FILES = $(MKINDEX) tclIndex

all: buildtclIndex

buildtclIndex: $(MKINDEX) $(TCL_SRCS)
	$(TCLSH) $(MKINDEX)

$(MKINDEX): Makefile
	echo "auto_mkindex . $(TCL_SRCS)" > $(MKINDEX)

tarfilelist:
	@srcs="Makefile $(OTHER_SRCS) lib" ; \
	for i in $$srcs ; do \
	  echo "$(CURRENT_DIR)/$$i" >> $(TOP)/fileslist ; \
	done

clean:
	$(RM) *~ $(CLEAN_FILES)
