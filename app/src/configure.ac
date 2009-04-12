#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(JEMRIS, 2.1, k.vahedipour@fz-juelich.de)
AC_CONFIG_HEADER([framework/config.h])

AC_LANG(C++)

# Check for the host type
AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CXX

# Check for required headers.
AC_CHECK_HEADERS([cstdlib],                                   [], [AC_MSG_ERROR([cannot find cstdlib, bailing out])])
AC_CHECK_HEADERS([ctime],                                     [], [AC_MSG_ERROR([cannot find ctime, bailing out])])
AC_CHECK_HEADERS([cmath],                                     [], [AC_MSG_ERROR([cannot find cmath, bailing out])])
AC_CHECK_HEADERS([climits],                                   [], [AC_MSG_ERROR([cannot find climits, bailing out])])
AC_CHECK_HEADERS([vector],                                    [], [AC_MSG_ERROR([cannot find vector, bailing out])])
AC_CHECK_HEADERS([iostream],                                  [], [AC_MSG_ERROR([cannot find iostream, bailing out])])
AC_CHECK_HEADERS([sstream],                                   [], [AC_MSG_ERROR([cannot find sstream, bailing out])])
AC_CHECK_HEADERS([fstream],                                   [], [AC_MSG_ERROR([cannot find fstream, bailing out])])
AC_CHECK_HEADERS([iomanip],                                   [], [AC_MSG_ERROR([cannot find iomanip, bailing out])])
AC_CHECK_HEADERS([map],                                       [], [AC_MSG_ERROR([cannot find map, bailing out])])
AC_CHECK_HEADERS([typeinfo],                                  [], [AC_MSG_ERROR([cannot find typeinfo, bailing out])])
AC_CHECK_HEADERS([cctype],                                    [], [AC_MSG_ERROR([cannot find cctype, bailing out])])
AC_CHECK_HEADERS([algorithm],                                 [], [AC_MSG_ERROR([cannot find algorithm, bailing out])])
AC_CHECK_HEADERS([math.h],                                    [], [AC_MSG_ERROR([cannot find math.h, bailing out])])
AC_CHECK_HEADERS([stdexcept],                                 [], [AC_MSG_ERROR([cannot find stdexcept, bailing out])])
AC_CHECK_HEADERS([cvode/cvode.h],                             [], [AC_MSG_ERROR([cannot find cvode/cvode.h, bailing out])])
AC_CHECK_HEADERS([nvector/nvector_serial.h],                  [], [AC_MSG_ERROR([cannot find nvector/nvector_serial.h, bailing out])])
AC_CHECK_HEADERS([var_array++.h],                             [], [AC_MSG_ERROR([cannot find vararray++.h, bailing out])])
AC_CHECK_HEADERS([ginac/ginac.h],                             [], [AC_MSG_ERROR([cannot find ginac/ginac.h, bailing out])])
AC_CHECK_HEADERS([cln/cln.h],                                 [], [AC_MSG_ERROR([cannot find cln/cln.h, bailing out])])
AC_CHECK_HEADERS([xercesc/dom/DOM.hpp],                       [], [AC_MSG_ERROR([cannot find xercesc/dom/DOM.hpp, bailing out])])
AC_CHECK_HEADERS([boost/math/special_functions/ellint_1.hpp], [], [AC_MSG_ERROR([cannot find boost/math/special_functions/ellint_1.hpp, bailing out])])
AC_CHECK_HEADERS([boost/math/special_functions/ellint_2.hpp], [], [AC_MSG_ERROR([cannot find boost/math/special_functions/ellint_2.hpp, bailing out])])

# Checks for libraries.
AC_CHECK_LIB([m],                   [cos],                    , [AC_MSG_ERROR([cannot find libm, bailing out])])
AC_CHECK_LIB([sundials_cvode],      [CVodeCreate],            , [AC_MSG_ERROR([cannot find libsundials_cvode, bailing out])])
AC_CHECK_LIB([sundials_nvecserial], [N_VNew_Serial],          , [AC_MSG_ERROR([cannot find libsundials_nvecserial, bailing out])])
AC_CHECK_LIB([var_array],           [vaDelete],               , [AC_MSG_ERROR([cannot find libvararray, bailing out])])

# Checks for C++ libraries with name spaces
m4_include(ac_check_lib_xercesc.m4)
m4_include(ac_check_lib_ginac.m4)

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
AC_CHECK_FUNCS([floor pow sqrt round])

#m4_include(ac_check_headers_mpi.m4)
m4_include(ac_prog_mpicxx.m4)
AC_PROG_MPICXX($mpicxx_list)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
