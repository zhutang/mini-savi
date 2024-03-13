#
# Makefile for SaVi - src directory. Run top-level Makefile instead.
#

# Used for the version string. Can be overriden per-platform.
     LINEEND = \\

include Makefile_defs_$(ARCH)


     SUBDIRS = include
       SHELL = /bin/sh
          RM = /bin/rm -f
          MV = /bin/mv
          CP = /bin/cp

     SAVIBIN = SaVi

# We retain . here to avoid cygwin appending .exe
     PROGRAM = $(SAVIBIN)-$(ARCH).bin
 INCLUDE_DIR = ./include


# remove -DNO_ZLIB from end to use zlib from http://www.gzip.org/zlib
# zlib enables transparent gzip compression of dynamic texture scratchfiles.
# OTHER_LIBS = -lz should be inserted at end of definitions file.

   GCC_FLAGS = -fcommon -O2 -DNO_ZLIB

        LIBS = $(TCL_LIBS) $(OTHER_LIBS) -lm


#    DEBUGFLAGS = -Wall -Wextra -Wconversion -pedantic -ansi

# These do not find anything to warn about:
#   DEBUGFLAGS = -Wshadow -Wpointer-arith -Wcast-qual \
#	-Wcast-align -Wwrite-strings -Wconversion -Waggregate-return \
#	-Wmissing-prototypes -Wmissing-declarations -Wnested-externs

# These do, but the reports aren't useful (e.g. use of all inlines):
# -ansi -Wtraditional

# For compiling a binary for use with gdb:
#   DEBUGFLAGS = -g

# On non-gcc compilers:
#   DEBUGFLAGS = fullwarn

        SRCS = \
		main.c \
		globals.c \
		satCmd.c \
		sats.c \
		modules.c \
		axes.c \
		sun.c \
		logo.c \
		orbits.c \
		cones.c \
		footprints.c \
		plane.c \
		earth.c \
		stars.c \
		coverage.c \
		coverage_vis.c \
		fisheye.c \
		time.c \
		Satellite.c \
		gv_init.c \
		satellites.c \
		orbit_utils.c \
		stats_utils.c \
		utils.c \
		gv_utils.c \
		tcl_utils.c

    INCLUDES = \
		$(INCLUDE_DIR)/Satellite.h \
		$(INCLUDE_DIR)/constants.h \
		$(INCLUDE_DIR)/coverage_vis.h \
		$(INCLUDE_DIR)/fisheye.h \
		$(INCLUDE_DIR)/globals.h \
		$(INCLUDE_DIR)/gv_utils.h \
		$(INCLUDE_DIR)/orbit_utils.h \
		$(INCLUDE_DIR)/sats.h \
		$(INCLUDE_DIR)/savi.h \
		$(INCLUDE_DIR)/stats_utils.h \
		$(INCLUDE_DIR)/tcl_utils.h \
		$(INCLUDE_DIR)/utils.h


   OTHER_SRCS = Makefile_defs_ Makefile_defs_linux Makefile_defs_cygwin \
		Makefile_defs_macosx Makefile_defs_freebsd \
		Makefile_defs_irix Makefile_defs_sun Makefile_defs_sun4 \
		Makefile_defs_ubuntu Makefile_defs_x86_64 \
		Makefile_defs_cygwin84

#
# commands and flags
#
    CFLAGS = $(GCC_FLAGS) $(DEBUGFLAGS) \
		-I$(INCLUDE_DIR) $(TCL_INCLUDES)


#
# derived objects
#
     OBJS = $(SRCS:.c=.o)

#
# targets
#

$(PROGRAM): $(OBJS) version.o
	$(CC) $(CFLAGS) -o $(PROGRAM) $(OBJS) version.o $(LIBS)
	$(MV) $(PROGRAM) ../bin/$(PROGRAM)
	$(CP) ../bin/$(PROGRAM) ../bin/$(SAVIBIN)

all:
	@$(MAKE_SUBDIRS:target=all)
	@make $(PROGRAM)

version.c: FORCE
# echo -E may be useful on Linux if output is over multiple lines
	@echo "char *Version = \"SaVi was compiled with 'make ARCH=$(ARCH)'$(LINEEND)\\nVersion" `cat ../VERSION`"$(LINEEND)\\nBuilt by $(USER)@$(HOST)$(HOSTNAME), `date`\";" > ./version.c


objs: $(OBJS)

$(OBJS): $(INCLUDES)

tarfilelist::
	@$(MAKE_SUBDIRS:target=tarfilelist)
	@srcs="Makefile $(SRCS) $(OTHER_SRCS)" ; \
	for i in $$srcs ; do \
	  echo "$(CURRENT_DIR)/$$i" >> $(TOP)/fileslist ; \
	done

clean:
	@$(MAKE_SUBDIRS:target=clean)
	$(RM) *~ *.o version.c core $(SAVIBIN)*

FORCE:

# macros

MAKE_SUBDIRS = \
	dirs="$(SUBDIRS)" ; \
	for i in $$dirs ; do \
	  (cd $$i; \
	   echo "making" target "in $(CURRENT_DIR)/$$i..."; \
	   $(MAKE) $(MFLAGS) target CURRENT_DIR=$(CURRENT_DIR)/$$i TOP=$(TOP)) ; \
	done
# END
