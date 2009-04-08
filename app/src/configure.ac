#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(JEMRIS, 2.1, k.vahedipour@fz-juelich.de)
AC_CONFIG_HEADER([framework/config.h])

# Check for the host type
AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Check for required headers.
# AC_CHECK_HEADERS([stdlib.h],                 [], [AC_MSG_ERROR([cannot find stdlib.h, bailing out])] )
# AC_CHECK_HEADERS([string.h],                 [], [AC_MSG_ERROR([cannot find string.h, bailing out])])
AC_CHECK_HEADERS([cvode/cvode.h],            [], [AC_MSG_ERROR([cannot find cvode/cvode.h, bailing out])])
AC_CHECK_HEADERS([nvector/nvector_serial.h], [], [AC_MSG_ERROR([cannot find nvector/nvector_serial.h, bailing out])])
   
# Checks for libraries.
AC_CHECK_LIB([cln],                 [main], , [AC_MSG_ERROR([cannot find libcln, bailing out])])
AC_CHECK_LIB([ginac],               [main], , [AC_MSG_ERROR([cannot find libgina, bailing out])])
AC_CHECK_LIB([m],                   [main], , [AC_MSG_ERROR([cannot find libm, bailing out])])
AC_CHECK_LIB([sundials_cvode],      [main], , [AC_MSG_ERROR([cannot find libsundials_cvode, bailing out])])
AC_CHECK_LIB([sundials_nvecserial], [main], , [AC_MSG_ERROR([cannot find libsundials_nvecserial, bailing out])])
AC_CHECK_LIB([var_array],           [main], , [AC_MSG_ERROR([cannot find libvararray, bailing out])])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_C_RESTRICT
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([floor pow sqrt])

AC_CHECK_HEADERS([mpi.h mpi++.h /usr/lib/mpich/include/mpi.h])
m4_include(ac_prog_mpicxx.m4)
AC_PROG_MPICXX($mpicxx_list)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
