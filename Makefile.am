SUBDIRS = libale doc tools

AM_CPPFLAGS = -I./libale

needed_libs = libale/libale.a

bin_PROGRAMS=smallpotato
smallpotato_SOURCES=src/unches.c src/system.h src/cmdline.c src/doIO.c src/communication.c \
    src/utils.c src/ai.c src/board.c src/pieces.c src/hash.c src/basicsearch.c \
    src/search.h src/eval.c src/eval.h src/common.h src/search.c src/board.h src/utils.h src/pieces.h \
    src/communication.h src/hash.h src/ai.h src/piecesstruct.h src/hashstruct.h src/heap.c src/heap.h \
    src/bench.h src/bench.c src/cmdline.h src/book.h src/book.c src/negascout.c src/moveorder.c src/moveorder.h

#  uncomment the following if unches requires the math library
smallpotato_LDADD=$(needed_libs) -lgdbm -lpthread -lm


EXTRA_DIST=THANKS BUGS aleweights aleweights.out sp.rc src/cmdline.cli

#  if you write a self-test script named `chk', uncomment the
#  following and add `chk' to the EXTRA_DIST list
#TESTS=chk

src/unches.o: src/cmdline.h
src/cmdline.o: src/cmdline.c
src/cmdline.c src/cmdline.h: src/cmdline.cli
	clig src/cmdline.cli


#$(PROGRAMS): $(needed_libs)

#  install the man page
#man_MANS=smallpotato.1
