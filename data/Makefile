#
# Makefile - SaVi data directory. Run top-level Makefile instead.
#
# $Id: Makefile,v 1.18 2016/11/19 07:15:52 lloydwood Exp $


       SHELL = /bin/sh
          RM = /bin/rm -f

  OTHER_SRCS = aries.tcl atcontact.tcl celestri.tcl clarke.tcl \
		commstellation-78.tcl deligo.tcl draim-4.tcl dmc.tcl \
		ellipso.tcl galileo.tcl globalstar.tcl glonass.tcl gps.tcl \
		gs2.tcl ico.tcl iridium-66.tcl leqo.tcl leosat.tcl macrocell.tcl \
		mm.tcl molnya.tcl nels.tcl nuonce.tcl o3b-networks.tcl \
		o3b-networks-full.tcl odyssey.tcl oneweb.tcl orbcomm.tcl \
		orblink.tcl quasi-geo.tcl rapideye.tcl sirius-radio.tcl \
		skybridge-64.tclskybridge-80.tcl spaceway-ngso.tcl \
		teledesic-288.tcl teledesic-840.tcl \
		tundra.tcl us-patent-6726152-boeing.tcl \
		viasat-ngso.tcl geo.tle visual.tle


all:

clean:
	$(RM) *~ $(CLEAN_FILES)

tarfilelist::
	@srcs="Makefile $(SRCS) $(OTHER_SRCS)" ; \
	for i in $$srcs ; do \
	  echo "$(CURRENT_DIR)/$$i" >> $(TOP)/fileslist ; \
	done

