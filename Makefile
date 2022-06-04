#
#	Makefile
#	This file is part of riss.
#	
#	26.08.2010
#	Copyright 2010 Norbert Manthey
#
DATE="`date +%y%m%d%H%M%S`"

#commandline
CMD = -O3
ARGS = 
SPACE = " "

#compiler, linker, archiver
CPP = g++
#	icpc -fast -Wall -DCOMPILE_INTEL
AR  = ar

#compiler flags
LIBS		=	
CPPINCLUDES 	= -I./include -I../libPD/src/runtime/include
COMMON_CPPFLAGS = $(CPPINCLUDES)
CPPFLAGS 	= $(COMMON_CPPFLAGS) -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -g3 -fno-strict-aliasing #-fno-exceptions -DNDEBUG #
LDFLAGS 	= -L. -lm -lpthread -ldl #-ligraph -static


#
# section below includes all the necessay rules for building riss
#

#build variables
SRCS = $(wildcard src/*.cc src/*/*.cc)
OBJS = $(SRCS:.cc=.o)
DEPS = $(SRCS:.cc=.d)
# for building the component libraries
DEPS += $(LIBSRCS:.cc=.d)

all: bva

#build solver
bva: $(OBJS)
	$(CPP) -o $@ $(OBJS) $(LIBS) $(LDFLAGS) $(CMD) $(CPPFLAGS) $(ARGS)

#build object file and dependencies files
.cc.o:
	$(CPP) $(CMD) -MD $(CPPFLAGS) $(ARGS) -c -o $@ $<

# create a tar from the source
tar: clean
	tar --exclude=".svn" --exclude=".kdev*" -czvf bva.tar.gz src include Makefile README LICENSE doc newMethod.sh

# clean up backups and old files
clean:
	rm -f *~ */*~ */*/*~
	rm -f $(OBJS) $(DEPS) $(LIBOBJS) $(LIBFILES) $(LIBRISSOBJ)
	rm -f log.txt
	rm -f bva
	cd doc; rm -fR html

# doxygen doc
doc:	clean
	cd doc; doxygen solver.config

# shows all todos in the sources and header
todo:
	clear && clear
	grep -n "TODO" src/*
	grep -n "TODO" include/*

fixme:	
	clear && clear
	grep -n "FIXME" src/*
	grep -n "FIXME" include/*
	
# include headerfile dependencies for sources
-include $(DEPS)

