#
# Makefile for SaVi - top level.
# $Id: Makefile,v 1.7 2011/02/21 11:35:35 lloydwood Exp $

     SUBDIRS = manual bin maps oogl data tcl src
       SHELL = /bin/sh
          RM = /bin/rm -f


  OTHER_SRCS = .geomview COPYRIGHT BUGS VERSION README configure savi \
	       README-COVERAGE-TEXTUREMAP PACKAGING

#
# targets
#

all:
	@$(MAKE_SUBDIRS:target=all)

clean:
	$(RM) *~ core $(CLEAN_FILES)
	@$(MAKE_SUBDIRS:target=clean)

tarfilelist:
	@srcs="Makefile $(SRCS) $(OTHER_SRCS)" ; \
	for i in $$srcs ; do \
	  echo "$(CURRENT_DIR)$$i" >> $(TOP)/fileslist ; \
	done
	@$(MAKE_SUBDIRS:target=tarfilelist)

# macros

MAKE_SUBDIRS = \
	dirs="$(SUBDIRS)" ; \
	for i in $$dirs ; do \
	  (cd $$i; \
	   echo "making" target "in $(CURRENT_DIR)$$i..."; \
	   $(MAKE) $(MFLAGS) target CURRENT_DIR=$(CURRENT_DIR)$$i TOP=$(TOP) ARCH=$(ARCH)) ; \
	done
