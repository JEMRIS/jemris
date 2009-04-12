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
    LIBS="${LIBS} -lxerces-3.0"
  else
    LIBS="-L${xerces_lib_base}/lib -lxerces-3.0 ${LIBS}"
  fi

  AC_TRY_LINK([#include <xercesc/util/PlatformUtils.hpp>],
     [XERCES_CPP_NAMESPACE_USE XMLPlatformUtils::Initialize(); return 0; ],
     [AC_MSG_RESULT([xerces-3.0])]
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
	[AC_DEFINE(XSEC_XERCES_REQUIRES_MEMMGR)]
	[AC_DEFINE(XSEC_XERCES_FORMATTER_REQUIRES_VERSION)],
	[AC_MSG_RESULT([no])])
	
AC_MSG_CHECKING([whether Xerces XMLString has ::release])

AC_TRY_COMPILE([#include <xercesc/util/XMLString.hpp>],
	[using namespace XERCES_CPP_NAMESPACE;
		XMLCh * tst;
		XMLString::release(&tst);
	],
	[AC_MSG_RESULT([yes])]
	[AC_DEFINE(XSEC_XERCES_XMLSTRING_HAS_RELEASE)],
	[AC_MSG_RESULT([no])])

AC_MSG_CHECKING([whether Xerces XMLElement has ::setIdAttribute(XMLCh*)])

AC_TRY_COMPILE([#include <xercesc/dom/DOM.hpp>],
	[using namespace XERCES_CPP_NAMESPACE;
		DOMElement * elt;
		elt->setIdAttribute(NULL);
	],
	[AC_MSG_RESULT([yes])]
    xerces_has_setidattribute=yes
	[AC_DEFINE(XSEC_XERCES_HAS_SETIDATTRIBUTE)],
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
	  [AC_DEFINE(XSEC_XERCES_HAS_BOOLSETIDATTRIBUTE)],
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
    [AC_DEFINE(XSEC_XERCES_DOMLSSERIALIZER)],
    [AC_MSG_RESULT([no])])

AC_MSG_CHECKING([whether Xerces DOMEntity uses getInputEncoding()])

AC_TRY_COMPILE([#include <xercesc/dom/DOM.hpp>],
    [using namespace XERCES_CPP_NAMESPACE;
        DOMEntity *t;
        t->getInputEncoding();
    ],
    [AC_MSG_RESULT([yes])]
    [AC_DEFINE(XSEC_XERCES_DOMENTITYINPUTENCODING)],
    [AC_MSG_RESULT([no])])
