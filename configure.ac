#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([rtprof],0.5.2)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(src/config.h)

AC_PROG_INSTALL

# Checks for programs.
AC_LANG_C
AC_PROG_CC
AC_PROG_MAKE_SET
AM_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([GL], [glBegin], [], [AC_MSG_ERROR([Missing libGL.])])
AC_CHECK_LIB([GLU], [gluSphere], [], [AC_MSG_ERROR([Missing libGLU.])])
AC_CHECK_LIB([SDL], [SDL_SetVideoMode], [], [AC_MSG_ERROR([Missing SDL.])])
AC_CHECK_LIB([bfd], [bfd_openr], [], [AC_MSG_ERROR([Missing binutils.])])
AC_CHECK_LIB([glut], [glutBitmapCharacter], [], [AC_MSG_ERROR([Missing glut.])])
AC_CHECK_LIB([m], [sqrt], [], [AC_MSG_ERROR([Missing libm(!).])])

CFLAGS="$CFLAGS -Werror"

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h netdb.h stdlib.h sys/socket.h sys/time.h unistd.h getopt.h, [], [AC_MSG_ERROR([Missing a header.])])
AC_CHECK_HEADERS(GL/gl.h GL/glu.h GL/glut.h GL/glx.h, [], [AC_MSG_ERROR([GL headers required.])])
AC_CHECK_HEADER(SDL/SDL.h, [], [AC_MSG_ERROR([SDL/SDL.h is required. http://www.libsdl.org])])
AC_CHECK_HEADER(bfd.h, [], [AC_MSG_ERROR([bfd.h is required. http://sources.redhat.com/binutils/])])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME

# Checks for library functions.
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(atexit floor gethostbyname gethostname gettimeofday memset socket sqrt strerror)

AC_CONFIG_FILES(Makefile src/Makefile src/rtprof/Makefile src/librtprof/Makefile)
AC_OUTPUT
