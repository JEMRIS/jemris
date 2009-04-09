AC_MSG_CHECKING([for GiNaC library])
OLD_LIBS=$LIBS
LIBS="${LIBS} -lginac"

AC_TRY_LINK([#include <ginac/ginac.h>],
   [GiNaC::ex::ex(); return 0; ],
   [AC_MSG_RESULT([ginac])]
   ginac_found=yes,
   [LIBS=$OLD_LIBS])

if test -z $ginac_found; then
    LIBS="${LIBS} -lginac"
fi


