#
# Makefile - SaVi src/include directory. Run top-level Makefile instead.
#
# $Id: Makefile,v 1.7 2009/05/18 07:59:57 lloydwood Exp $

       SHELL = /bin/sh
          RM = /bin/rm -f

        SRCS = Satellite.h globals.h orbit_utils.h stats_utils.h \
		constants.h gv_file.h gv_utils.h sats.h utils.h \
		tcl_utils.h int_types.h savi.h coverage_vis.h \
		fisheye.h time.h

all:

clean:
	$(RM) *~ $(CLEAN_FILES)

tarfilelist:
	@srcs="Makefile $(SRCS) $(OTHER_SRCS)" ; \
	for i in $$srcs ; do \
	  echo "$(CURRENT_DIR)/$$i" >> $(TOP)/fileslist ; \
	done

