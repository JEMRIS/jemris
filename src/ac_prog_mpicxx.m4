AC_DEFUN([AC_PROG_MPICXX], [
AC_ARG_VAR([MPICXX],  [MPI C++ compiler command])
AC_ARG_VAR([PJEMRIS], [Parallel JEMRIS target])

# find compiler and characteristics
AC_CHECK_PROGS([MPICXX], [$1], [none])
if test x$MPICXX = xnone; then
  AC_MSG_ERROR([MPI C++ compiler not found])
fi

# find compiler and characteristics
 AC_MSG_CHECKING(for mpi.h)
  AC_PREPROC_IFELSE(
  [AC_LANG_SOURCE([[#include <mpi.h>]])],
  [ 
    AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_MPI_H,1,[define that mpi is being used])
  ],[AC_MSG_RESULT(no)])

cat >>conftest.cpp <<_ACEOF
#include <mpi.h>
int main(int argc, char** argv) {
  MPI::Init(argc, argv);
  MPI::Finalize();
  return 0;
}
_ACEOF

AC_MSG_CHECKING([whether the MPI C++ compiler works])
echo "$MPICXX -o conftest conftest.cpp" >&5
$MPICXX -o conftest conftest.cpp 2>&5
if test $? -ne 0; then
  AC_MSG_FAILURE([failed!])
else
  AC_MSG_RESULT([yes])
  PJEMRIS=pjemris
fi

ic_mpirun_found=no
if test x$2 = xyes; then
  AC_MSG_CHECKING([for MPI run command])
  case "$MPICXX" in
#    *mpCC*)
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
#    *tmCC*)
    *mpCC*)
      mpirun="mprun"
      mpirun_proc_opt="-np"
      ;;
    *)
      AC_MSG_ERROR([unknown MPI C++ compiler])
      ;;
  esac
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

