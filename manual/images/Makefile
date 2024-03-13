#
# Makefile - savi manual/images directory. Run top-level Makefile instead.
#
# $Id: Makefile,v 1.5 2016/08/31 07:03:03 lloydwood Exp $

        SHELL = /bin/sh
           RM = /bin/rm -f

   OTHER_SRCS = savi-choose.png savi-constants.png savi-coverage.png \
	        savi-edit.png savi-empty.png savi-fisheye.png savi-main.png \
		savi-rosette.png savi-spot.png savi-tle.png

all:

clean:
	$(RM) *~ $(CLEAN_FILES)

tarfilelist:
	@srcs="Makefile $(SRCS) $(OTHER_SRCS)" ; \
	for i in $$srcs ; do \
	  echo "$(CURRENT_DIR)/$$i" >> $(TOP)/fileslist ; \
	done

