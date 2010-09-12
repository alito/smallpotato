SUBDIRS = libale doc tools

INCLUDES = -I./libale

needed_libs = libale/libale.a

bin_PROGRAMS=smallpotato
smallpotato_SOURCES=unches.c system.h aclocal.m4 smallpotato.1 cmdline.c doIO.c communication.c \
	utils.c ai.c board.c pieces.c hash.c basicsearch.c \
	search.h eval.c eval.h common.h search.c board.h utils.h pieces.h \
	communication.h hash.h ai.h piecesstruct.h hashstruct.h heap.c heap.h \
	bench.h bench.c cmdline.h book.h book.c negascout.c moveorder.c moveorder.h

#  uncomment the following if unches requires the math library
smallpotato_LDADD=$(needed_libs) -lgdbm -lpthread -lm

EXTRA_DIST=smallpotato.lsm.in smallpotato.spec.in THANKS BUGS \
	cmdline.cli aleweights aleweights.out sp.rc

#  if you write a self-test script named `chk', uncomment the
#  following and add `chk' to the EXTRA_DIST list
#TESTS=chk

unches.o: cmdline.h
cmdline.o: cmdline.c
cmdline.c cmdline.h: cmdline.cli
	clig cmdline.cli

#$(PROGRAMS): $(needed_libs)

# clig does not support .info pages
# info_TEXINFOS = unches.texinfo
# unches_TEXINFOS = gpl.texinfo

#  install the man page
man_MANS=smallpotato.1
