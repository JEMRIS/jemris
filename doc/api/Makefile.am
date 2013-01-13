apidir = $(DESTDIR)$(docdir)/api
resdir = $(apidir)/res

static_docs = res/doxygen.css res/tabs.css
DOXY_STAMP = doxygen-build.stamp
DOXY_FILE = doxy-codeare.conf

EXTRA_DIST = $(static_docs) $(DOXY_FILE).in
api_DATA = $(EXTRA_DIST) doxygen-build.stamp ../../src/doc/mainpage.ixx

# Generate local API documentation?
if BUILD_API


doxygen-build.stamp: $(DOXY_FILE) $(top_srcdir)/src/*.hpp $(top_srcdir)/src/matrix/*.hpp $(top_srcdir)/src/misc/*.hpp ../../src/doc/mainpage.ixx ../../src/doc/matrix.ixx
	doxygen $(DOXY_FILE)
	touch doxygen-build.stamp

install-data-local: doxygen-build.stamp
	$(mkinstalldirs) $(apidir)
	$(mkinstalldirs) $(resdir)
	if test -d api; then \
	  for dir in api/*; do \
	    if test -d $$dir; then \
	      b=`basename $$dir`; \
	      $(mkinstalldirs) $(apidir)/$$b; \
	      for f in $$dir/*; do \
	        if test -f $$f; then \
		      $(INSTALL_DATA) $$f $(apidir)/$$b; \
	        fi \
	      done \
	    fi \
	  done \
	fi; 

uninstall-local:
	rm -rf $(apidir)

clean-local:
	if test -d api; then rm -rf api; fi
	if test -f doxygen-build.stamp; then rm -f doxygen-build.stamp; fi

else 

doxygen-build.stamp:

endif