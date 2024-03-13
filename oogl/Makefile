#
# Makefile - savi oogl directory. Run top-level Makefile instead.
#
# $Id: Makefile,v 1.13 2016/09/03 06:49:39 lloydwood Exp $

        SHELL = /bin/sh
           RM = /bin/rm -f

   OTHER_SRCS = savi.oogl savilogo.oogl \
		stars_h.oogl stars.oogl ambient_light.oogl \
		earth_h.oogl earth.oogl earth_fancy_h.oog earth_fancy.oogl \
		Earth.ppm.Z blue_marble_land_shallow_topo_2048.jpeg \
		earth_vect_h.oogl earth.vect \
	        earth_texture_h.oogl \
		earth_coverage_h.oogl earth_coverage.oogl \
	        earth_coverage_unp_h.oogl earth_coverage_unp.oogl

all:

clean:
	$(RM) *~ $(CLEAN_FILES)

tarfilelist:
	@srcs="Makefile $(SRCS) $(OTHER_SRCS)" ; \
	for i in $$srcs ; do \
	  echo "$(CURRENT_DIR)/$$i" >> $(TOP)/fileslist ; \
	done
