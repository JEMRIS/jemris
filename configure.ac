AC_PREREQ(2.61)
AC_INIT(JEMRIS, [2.6], k.vahedipour@fz-juelich.de)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADER([src/config.h])
AM_INIT_AUTOMAKE([foreign])
define([svnversion], esyscmd([sh -c "svnversion|tr -d '\n'"]))dnl
AC_DEFINE(SVN_REVISION, "svnversion", [SVN Revision])

# C++ project
AC_LANG(C++)

# Check standard headers
AC_HEADER_STDC

# Check for the host type
AC_CANONICAL_HOST

AC_ARG_ENABLE([documentation],
     [  --enable-documentation          turn on documentation C++ library support (default: yes)],
     [case "${enableval}" in
       yes) documentation=true ;;
       no)  documentation=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-documentation]) ;;
     esac],[documentation=false])
     AM_CONDITIONAL([DOCUMENTATION], [test x$documentation = xtrue])

# Check for xsltproc & xmllint ()
if test x$documentation = xtrue; then

build_docs=yes

XSLTPROC=
AC_CHECK_PROGS([XSLTPROC], [xsltproc])
if test -z "$XSLTPROC"; then
  AC_MSG_WARN([xsltproc (from the libxslt source package) not found, not building documentation])
  build_docs=no
fi

build_api=yes

DOXYGEN=
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN"; then
  AC_MSG_WARN([doxygen not found, not building documentation])
  build_api=no
fi

fi

AM_CONDITIONAL(BUILD_DOCS, test x$build_docs = xyes)
AM_CONDITIONAL(BUILD_API,  test x$build_api  = xyes)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# OpenMP
#AC_OPENMP

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
AC_CHECK_HEADERS([cvode.h],             [], [
    AC_CHECK_HEADERS([cvode/cvode.h],   [], [AC_MSG_ERROR([cannot find neither cvode.h nor cvode/cvode.h, bailing out])])
])
AC_CHECK_HEADERS([nvector_serial.h],    [], [
    AC_CHECK_HEADERS([nvector/nvector_serial.h], [], [AC_MSG_ERROR([cannot find neither nvector_serial.h nor nvector/nvector_serial.h, bailing out])])
])
AC_CHECK_HEADERS([ginac/ginac.h],                             [], [AC_MSG_ERROR([cannot find ginac/ginac.h, bailing out])])
AC_CHECK_HEADERS([cln/cln.h],                                 [], [AC_MSG_ERROR([cannot find cln/cln.h, bailing out])])
AC_CHECK_HEADERS([xercesc/dom/DOM.hpp],                       [], [AC_MSG_ERROR([cannot find xercesc/dom/DOM.hpp, bailing out. Get it from http://xerces.apache.org/xerces-c.])])

AC_ARG_ENABLE([boost],
     [  --enable-boost          turn on boost C++ library support (default: yes)],
     [case "${enableval}" in
       yes) boost=true ;;
       no)  boost=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-boost]) ;;
     esac],[boost=true])
     AM_CONDITIONAL([BOOST], [test x$boost = xtrue])

if test x$boost = xtrue; then
	AC_CHECK_HEADERS(
		[boost/math/special_functions/ellint_1.hpp boost/math/special_functions/ellint_2.hpp],
		[AC_DEFINE(HAVE_BOOST,1,[boost is available])],
    	[AC_MSG_RESULT([cannot find boost ellint_1.hpp and/or ellint_2.hpp, Biot-Savart-Loop not available])])
fi
AC_CHECK_HEADERS([H5Cpp.h],          [], [AC_MSG_RESULT([cannot find H5Cpp.h, hdf5 dump / read functionality unavailable!])])

# Checks for libraries.
AC_CHECK_LIB([m],                   [cos],                    , [AC_MSG_ERROR([cannot find libm, bailing out])])
AC_CHECK_LIB([dl],                  [dlopen],                 , [AC_MSG_ERROR([cannot find libdl, bailing out])])
AC_CHECK_LIB([sundials_cvode],      [CVodeCreate],            , [AC_MSG_ERROR([cannot find libsundials_cvode, bailing out])])
AC_CHECK_FUNC([CVodeSetUserData],      [AC_DEFINE([CVODE26],  [1], [CVODE Version >= 2.6?])])
AC_CHECK_LIB([sundials_nvecserial], [N_VNew_Serial],          , [AC_MSG_ERROR([cannot find libsundials_nvecserial, bailing out])])


# Checks for C++ libraries with name spaces
AC_MSG_CHECKING([for GiNaC library])
OLD_LIBS=$LIBS
LIBS="${LIBS} -lginac -lcln"

AC_TRY_LINK([#include <ginac/ginac.h>],
   [GiNaC::ex* mex = new GiNaC::ex();],
   [AC_MSG_RESULT([ginac])]
   ginac_found=yes,
   [LIBS=$OLD_LIBS]
   [AC_MSG_ERROR([cannot find libginac, bailing out])])

if test -z $ginac_found; then
    LIBS="${LIBS} -lginac -lcln"
fi

AC_MSG_CHECKING([for Xerces 2.x or 3.x library naming convention])
OLD_LIBS=$LIBS
if test -z xerces_lib_base; then
  LIBS="${LIBS} -lxerces-c"
else
  LIBS="-lxerces-c ${LIBS}"
fi

AC_TRY_LINK([#include <xercesc/util/PlatformUtils.hpp>],
   [XERCES_CPP_NAMESPACE_USE XMLPlatformUtils::Initialize(); return 0; ],
   [AC_MSG_RESULT([xerces-c])]
   xerceslib_found=yes,
   [LIBS=$OLD_LIBS])

if test -z $xerceslib_found; then
  if test -z xerces_lib_base; then
    LIBS="${LIBS} -lxerces-c "
  else
    LIBS="-L${xerces_lib_base}/lib -lxerces-c ${LIBS}"
  fi

  AC_TRY_LINK([#include <xercesc/util/PlatformUtils.hpp>],
     [XERCES_CPP_NAMESPACE_USE XMLPlatformUtils::Initialize(); return 0; ],
     [AC_MSG_RESULT([xerces-c])]
     xerceslib_found=yes,
     [AC_MSG_RESULT([unable to determine])]
     [AC_MSG_ERROR([cannot link to Xerces library])])
fi

# Find out some properties of the version of Xerces we have

AC_MSG_CHECKING([whether Xerces XMLFormatter requires a version])

AC_TRY_COMPILE([#include <xercesc/framework/XMLFormatter.hpp>
	#include <xercesc/util/XercesDefs.hpp>
	#include <xercesc/util/XMLUniDefs.hpp>],
	[using namespace XERCES_CPP_NAMESPACE;
	  XMLCh s_encoding[] = {
		chLatin_U, chLatin_T, chLatin_F, chDash, chDigit_8, chNull
	  };
	  XMLFormatter f(s_encoding,
		NULL,
		NULL, 
		XMLFormatter::NoEscapes, 
		XMLFormatter::UnRep_CharRef);
	],
	[AC_MSG_RESULT([yes])]
	[AC_DEFINE([XSEC_XERCES_REQUIRES_MEMMGR],[],[Description])]
	[AC_DEFINE([XSEC_XERCES_FORMATTER_REQUIRES_VERSION],[],[Description])],
	[AC_MSG_RESULT([no])])
	
AC_MSG_CHECKING([whether Xerces XMLString has ::release])

AC_TRY_COMPILE([#include <xercesc/util/XMLString.hpp>],
	[using namespace XERCES_CPP_NAMESPACE;
		XMLCh * tst;
		XMLString::release(&tst);
	],
	[AC_MSG_RESULT([yes])]
	[AC_DEFINE([XSEC_XERCES_XMLSTRING_HAS_RELEASE],[],[Description])],
	[AC_MSG_RESULT([no])])

AC_MSG_CHECKING([whether Xerces XMLElement has ::setIdAttribute(XMLCh*)])

AC_TRY_COMPILE([#include <xercesc/dom/DOM.hpp>],
	[using namespace XERCES_CPP_NAMESPACE;
		DOMElement * elt;
		elt->setIdAttribute(NULL);
	],
	[AC_MSG_RESULT([yes])]
    xerces_has_setidattribute=yes
	[AC_DEFINE([XSEC_XERCES_HAS_SETIDATTRIBUTE],[],[Description])],
	[AC_MSG_RESULT([no])])

if test -z $xerces_has_setidattribute; then

  AC_MSG_CHECKING([whether Xerces XMLElement has ::setIdAttribute(XMLCh*, bool)])
  AC_TRY_COMPILE([#include <xercesc/dom/DOM.hpp>],
	  [using namespace XERCES_CPP_NAMESPACE;
		  DOMElement * elt;
		  elt->setIdAttribute(NULL, false);
	  ],
	  [AC_MSG_RESULT([yes])]
      xerces_has_setidattribute=yes
	  [AC_DEFINE([XSEC_XERCES_HAS_BOOLSETIDATTRIBUTE],[],[Description])],
	  [AC_MSG_RESULT([no])])

fi

# For Xerces 3.x we now have a stricter DOM L3 implementation
AC_MSG_CHECKING([whether Xerces DOMImplementationLS has DOMLSSerializer])

AC_TRY_COMPILE([#include <xercesc/dom/DOM.hpp>],
    [using namespace XERCES_CPP_NAMESPACE;
        DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(NULL);
        DOMLSSerializer *ls = ((DOMImplementationLS*)impl)->createLSSerializer();
    ],
    [AC_MSG_RESULT([yes])]
    [AC_DEFINE(XSEC_XERCES_DOMLSSERIALIZER,[],[Description])],
    [AC_MSG_RESULT([no])])

AC_MSG_CHECKING([whether Xerces DOMEntity uses getInputEncoding()])

AC_TRY_COMPILE([#include <xercesc/dom/DOM.hpp>],
    [using namespace XERCES_CPP_NAMESPACE;
        DOMEntity *t;
        t->getInputEncoding();
    ],
    [AC_MSG_RESULT([yes])]
    [AC_DEFINE(XSEC_XERCES_DOMENTITYINPUTENCODING,[],[Description])],
    [AC_MSG_RESULT([no])])


# HDF5. Not mandatory. 

# Mac OSX needs libsz
#case "$host" in
#  *darwin*)
#  AC_CHECK_LIB([sz],           [SZ_Compress], [], [AC_MSG_ERROR([cannot find libsz, bailing out])])
#;;
#esac

AC_ARG_WITH(hdf5,[  --with-hdf5             which hdf5 library to use],[
case $withval in
  yes)
    HDF5=' -lhdf5_cpp -lhdf5 -ldl'
    ;;
  no)
    HDF5=''
    ;;
  *)
    HDF5=$withval
    ;;
esac
])

AC_MSG_CHECKING([for H5File in -lhdf5_cpp -lhdf5])
OLD_LIBS=$LIBS
LIBS="${LIBS} -lhdf5_cpp -lhdf5 -ldl"
AC_TRY_LINK([#include <H5Cpp.h>],
   [H5::H5File f ("t.h5", H5F_ACC_RDONLY);],
   [AC_MSG_RESULT([yes])]
   hdf5_found=yes,
   [LIBS=$OLD_LIBS]
   [AC_MSG_RESULT([no. Cannot find libhdf5_cpp, hdf5 dump/read not available.])])
if test "$hdf5_found" = yes; then
   AC_DEFINE([HAVE_HDF5], [1], [HDF5 found])
   HDF5="-lhdf5_cpp -lhdf5"
   AC_SUBST(HDF5)
fi

case "$target" in 
*-mingw*) 
AC_CHECK_LIB([iberty],                   [mkstemps],                    , [AC_MSG_ERROR([cannot find libiberty, bailing out])])
AC_DEFINE(HAVE_MKSTEMPS, [], [Description])
esac

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_C_RESTRICT
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([floor pow sqrt round])

AC_ARG_ENABLE([mpi],
     [  --enable-mpi            turn on parallel jemris (default: yes) ],
     [case "${enableval}" in
       yes) mpi=true ;;
       no)  mpi=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-mpi]) ;;
     esac],[mpi=true])
     AM_CONDITIONAL([MPI], [test x$mpi = xtrue])


AC_DEFUN([AC_PROG_MPICXX], [
AC_ARG_VAR([MPICXX],  [MPI C++ compiler command])
AC_ARG_VAR([MPIRUN],  [MPI run command])

# find compiler and characteristics
AC_CHECK_PROGS([MPICXX], [$1], [none])
if test x$MPICXX = xnone; then
  AC_MSG_ERROR([MPI C++ compiler not found])
fi

# find compiler and characteristics
#AC_MSG_CHECKING(for mpi.h)
#  AC_PREPROC_IFELSE(
#  [AC_LANG_SOURCE([[#include <mpi.h>]])],
#  [ 
#    AC_MSG_RESULT(yes)
#    AC_DEFINE(HAVE_MPI_H,1,[define that mpi is being used])
#  ],[AC_MSG_RESULT(no)])

cat >>conftest.cpp <<_ACEOF
#include <mpi.h>
int main(int argc, char** argv) {
  MPI::Init(argc, argv);
  MPI::COMM_WORLD.Get_size();
  MPI::Finalize();
  return 0;
}
_ACEOF

cat >>conftest2.cpp <<_ACEOF2
#include <mpi.h>
int main(int argc, char** argv) {
  MPI::Init_thread(MPI_THREAD_MULTIPLE);
  MPI::COMM_WORLD.Get_size();
  MPI::Finalize();
  return 0;
}
_ACEOF2

AC_MSG_CHECKING([whether the MPI C++ compiler works])
echo "$MPICXX $CXXFLAGS -o conftest conftest.cpp" >&5
$MPICXX $CXXFLAGS -o conftest conftest.cpp 2>&5
if test $? -ne 0; then
  AC_MSG_FAILURE([failed!])
else
  mpiw=yes
  AC_MSG_RESULT([yes])
fi

AC_MSG_CHECKING([whether MPI library supports threads])
echo "$MPICXX $CXXFLAGS -o conftest2 conftest2.cpp" >&5
$MPICXX $CXXFLAGS -o conftest2 conftest2.cpp 2>&5
if test $? -ne 0; then
  AC_MSG_RESULT([no])
else
  AC_MSG_RESULT([yes])
  AC_DEFINE(HAVE_MPI_THREADS,1,[define that mpi supports threads])
fi
rm -f conftest2.*

ic_mpirun_found=no
if test x$mpiw = xyes; then
  AC_MSG_CHECKING([for MPI run command])
  if test x$ac_cv_env_MPIRUN_value = x; then
  case "$MPICXX" in
    *mpCC_r*)
      mpirun="poe"
      mpirun_proc_opt="-procs"
      ;;
    *mpiCC*)
      mpirun="mpirun"
      mpirun_proc_opt="-np"
      ;;
    *hCC*)
      mpirun="mpiexec"
      mpirun_proc_opt="-n"
      ;;
    *mpCC*)
      mpirun="mprun"
      mpirun_proc_opt="-np"
      ;;
    *mpicxx*)
      mpirun="mpirun"
      mpirun_proc_opt="-np"
      ;;
    *)
      AC_MSG_ERROR([unknown MPI C++ compiler])
      ;;
  esac
  else
	  mpirun=$ac_cv_env_MPIRUN_value
	  mpirun_proc_opt="-np"
  fi
  AC_MSG_RESULT([$mpirun])
  AC_SUBST(MPIRUN, [$mpirun])
  AC_SUBST(MPIRUN_PROC_OPT, [$mpirun_proc_opt])

  AC_MSG_CHECKING([whether the MPI run command works])
  if test $mpirun = poe; then
    mpicmd="$mpirun $PWD/conftest $mpirun_proc_opt 2"
  else
    mpicmd="$mpirun $mpirun_proc_opt 2 $PWD/conftest"
  fi
  echo $mpicmd >&5
  $mpicmd 2>&5
  if test $? -ne 0; then
    AC_MSG_RESULT([no, will skip MPI and P++ tests])
  else
    AC_MSG_RESULT([yes])
    ic_mpirun_found=yes
  fi
fi
rm -f conftest.*
])

case "$host" in 
  *aix*)
    cc_list="xlc_r gcc"
    f77_list="xlf_r"
    cxx_list="xlC_r g++"
    f90_list="xlf90_r"
    mpicc_list="mpcc_r mpicc"
    mpif77_list="mpxlf_r mpif77"
    mpicxx_list="mpCC_r mpiCC"
    gm4_list="gm4 m4"
    ;;
  *darwin*)
    cc_list="gcc"
    f77_list="xlf"
    cxx_list="g++"
    f90_list="xlf90"
    mpicc_list="mpicc"
    mpif77_list="mpif77"
    mpicxx_list="mpicxx"
    ;;
  *linux*)
    cc_list="gcc pgcc icc"
    f77_list="pgf77 ifort lf95 f95"
    cxx_list="g++ pgCC icpc"
    f90_list="pgf90 ifort lf95 f95"
    mpicc_list="mpicc hcc"
    mpif77_list="mpif90 mpif77 h77"
    mpicxx_list="mpicxx mpiCC hCC"
    gm4_list="gm4 m4"
    ;;
  *sun*)
    cc_list="cc gcc"
    f77_list="f77"
    cxx_list="CC g++"
    f90_list="f90"
    mpicc_list="mpcc mpicc"
    mpif77_list="mpf77 mpif77"
    mpicxx_list="mpCC mpiCC"
    gm4_list="gm4 m4"
    ;;
esac

if test x$mpi = xtrue; then
  AC_PROG_MPICXX($mpicxx_list)
  CXX=$MPICXX
fi

AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile doc/userguide/Makefile doc/userguide/doxy-jemris.conf doc/site/Makefile doc/site/nav.js share/Makefile share/examples/Makefile share/examples/approved/Makefile share/matlab/Makefile share/matlab/JEMRIS_sim.m share/matlab/JEMRIS_seq.m share/matlab/JEMRIS_txrx.m share/matlab/pbs_script.sh])

AC_OUTPUT
