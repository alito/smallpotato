#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([Small Potato], [0.6.1], [alito@organicrobot.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([unches.c])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB([gdbm], [gdbm_open],[AC_CHECK_HEADER([gdbm.h],,[AC_MSG_ERROR([Could not find gdbm.h])])],
[AC_MSG_ERROR([Need libgdbm for the opening book])])
AC_CHECK_LIB([m], [log],,[AC_MSG_ERROR([No math library?])])
AC_CHECK_LIB([pthread], [pthread_create],,[AC_MSG_ERROR(Need the pthread threading library)])

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_MAJOR
AC_CHECK_HEADERS([error.h fcntl.h float.h libintl.h limits.h locale.h memory.h stdlib.h string.h strings.h sys/file.h sys/param.h sys/time.h unistd.h utime.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STAT
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_SETVBUF_REVERSED
AC_FUNC_STRTOD
AC_CHECK_FUNCS([gettimeofday memchr memset strchr strrchr strtol])


AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 libale/Makefile
                 tools/Makefile])
AC_OUTPUT
