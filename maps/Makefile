#
# Makefile - SaVi maps directory. Run top-level Makefile instead.
#
# $Id: Makefile,v 1.7 2014/12/24 16:17:55 lloydwood Exp $

       SHELL = /bin/sh
          RM = /bin/rm -f

  OTHER_SRCS = world.cyl.pbm world1024.cyl.pbm \
	       world.unp.pbm world1024.unp.pbm \
	       world-mask.unp.pbm world-mask1024.unp.pbm \
	       world.sph.pbm world1024.sph.pbm \
	       world90W.sph.pbm world90W1024.sph.pbm \
	       world.sinu.pbm world1024.sinu.pbm \
	       world90W.sinu.pbm world90W1024.sinu.pbm \
	       savi-logo.gif

all:

clean:
	$(RM) *~ $(CLEAN_FILES)

tarfilelist::
	@srcs="Makefile $(SRCS) $(OTHER_SRCS)" ; \
	for i in $$srcs ; do \
	  echo "$(CURRENT_DIR)/$$i" >> $(TOP)/fileslist ; \
	done
