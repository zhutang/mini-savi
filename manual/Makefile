#
# Makefile - savi manual directory. Run top-level Makefile instead.
#
# $Id: Makefile,v 1.4 2006/09/18 13:56:05 lloydwood Exp $

      SUBDIRS = images
        SHELL = /bin/sh
           RM = /bin/rm -f

   OTHER_SRCS = index.html

all:
	@$(MAKE_SUBDIRS:target=all)

clean:
	@$(MAKE_SUBDIRS:target=clean)
	$(RM) *~ $(CLEAN_FILES)

tarfilelist:
	@$(MAKE_SUBDIRS:target=tarfilelist)
	@srcs="Makefile $(SRCS) $(OTHER_SRCS)" ; \
	for i in $$srcs ; do \
	  echo "$(CURRENT_DIR)/$$i" >> $(TOP)/fileslist ; \
	done

# macros

MAKE_SUBDIRS = \
        dirs="$(SUBDIRS)" ; \
        for i in $$dirs ; do \
          (cd $$i; \
           echo "making" target "in $(CURRENT_DIR)/$$i..."; \
           $(MAKE) $(MFLAGS) target CURRENT_DIR=$(CURRENT_DIR)/$$i TOP=$(TOP)) ; \
        done
# END
