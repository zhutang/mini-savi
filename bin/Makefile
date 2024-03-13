#
# Makefile - SaVi bin directory. Run top-level Makefile instead.
#
# $Id: Makefile,v 1.3 2011/02/17 16:55:30 lloydwood Exp $

    TOP = ../..
     RM = /bin/rm -f

SAVIBIN = SaVi

all:

clean:
	$(RM) *~ core $(SAVIBIN)*

tarfilelist::
	@srcs="Makefile $(SRCS) $(OTHER_SRCS)" ; \
	for i in $$srcs ; do \
	  echo "$(CURRENT_DIR)/$$i" >> $(TOP)/fileslist ; \
	done

