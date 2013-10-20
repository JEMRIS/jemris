instdir = $(DESTDIR)$(docdir)

DOXY_STAMP = doxygen-build.stamp
DOXY_FILE = doxy.conf

EXTRA_DIST = $(static_docs) $(DOXY_FILE).in
doc_DATA = $(EXTRA_DIST) doxygen-build.stamp 

# Generate local API documentation?
if BUILD_DOCS

doxygen-build.stamp: $(DOXY_FILE) $(top_srcdir)/src/*.h userguide/*.dox
	doxygen $(DOXY_FILE)
	touch doxygen-build.stamp

install-data-local: doxygen-build.stamp
	$(mkinstalldirs) $(instdir)
	if test -d html; then \
	  for f in html/*; do \
	    if test -f $$f; then \
		  $(INSTALL_DATA) $$f $(instdir)/; \
	    fi \
	  done; \
	  for dir in html/*; do \
	    if test -d $$dir; then \
	      b=`basename $$dir`; \
	      $(mkinstalldirs) $(instdir)/$$b; \
	      for f in $$dir/*; do \
	        if test -f $$f; then \
		      $(INSTALL_DATA) $$f $(instdir)/$$b; \
	        fi \
	      done \
	    fi \
	  done \
	fi; 

uninstall-local:
	rm -rf $(instdir)

clean-local:
	if test -d html; then rm -rf html; fi
	if test -f doxy-build.stamp; then rm -f doxy-build.stamp; fi
	if test -f doxy.conf; then rm -f doxy.conf; fi

else 

doxygen-build.stamp:

endif

