# Generate local documentation?
if BUILD_DOCS

RESOURCES = nav.js 
#screen.css sh_style.css sh_main.js sh_cpp.js sh_xml.js sh_sh.js nav.js
PAGES = $(patsubst xml/%.xml,%.html,$(wildcard xml/*.xml))
%.html: xml/%.xml $(STYLESHEET)
	$(XSLTPROC) $(STYLESHEET) $< > $@

endif

EXTRA_DIST=$(PAGES) $(RESOURCES)

sitedir = $(DESTDIR)$(docdir)
site_DATA = $(EXTRA_DIST)

XSLTPROC = xsltproc --xinclude --nonet
STYLESHEET = xsl/jemris.org.xsl

# Generate local documentation?
if BUILD_DOCS

%.html: xml/%.xml
	$(XSLTPROC) $(STYLESHEET) $< > $@

endif
