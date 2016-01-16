
AUTOMAKE_OPTIONS = foreign

SUBDIRS =	bufferpool \
		socket \
		md5 \
		log \
		multicast \
		command_environment \
		eventloop \
		rtp \
		rtcp \
		intnet \
		rtsp \
		mediainfo \
		sdp \
		schedule \
		src \
		docs \
		etc \
		avroot
		

EXTRA_DIST = 	include \
		config \
		INSTALL-FAST

FENICE_AVROOT_DIR = $(DESTDIR)@fenice_avroot_dir@
FENICE_LOG_FILE = `basename @fenice_log_file@`
		
install-exec-hook:
	@echo -n "Installing Log file...";\
	test ! -d $(DESTDIR)$(localstatedir)/log && mkdir -p $(DESTDIR)$(localstatedir)/log || :;\
	test ! -f $(DESTDIR)@fenice_log_file@ && touch $(DESTDIR)@fenice_log_file@ || :;\
	test ! -f $(FENICE_AVROOT_DIR)/../$(FENICE_LOG_FILE) && ln -s $(DESTDIR)@fenice_log_file@ $(FENICE_AVROOT_DIR)/../$(FENICE_LOG_FILE) || :;\
	echo -n " ($(DESTDIR)@fenice_log_file@)";\
	echo " done.";

dist-hook:
	@for svn in `find $(distdir) -iname .svn`; do \
		rm -rf $$svn; \
	done

distclean-local:
	@if test -d autom4te.cache; then \
		rm -rf autom4te.cache; \
	fi

MAINTAINER_FILES = acinclude.m4 aclocal.m4 config.h.in* configure Makefile.in mkinstalldirs error.log stamp-h.in
MAINTAINER_DIRS = config

maintainer-clean-local:
	@for mfile in $(MAINTAINER_FILES); do \
		test -f $$mfile && rm -f $$mfile; \
	done;\
	for mdir in $(MAINTAINER_DIRS); do \
		test -d $$mdir && rm -rf $$mdir; \
	done;\
	find -iname Makefile.in -exec rm \{} \;

svnclean: maintainer-clean
	@SVNREV_DEFINE=`grep -sE '#define +SVNREV +\" *\\\$$ *Rev.* *\\\$$ *\"' svnrev.h`; \
	test -z "$$SVNREV_DEFINE" && SVNREV_DEFINE="#define SVNREV \"\$$Rev\$$\"" || :; \
	echo -e "/* This header is just for keeping track of current SVN revision.\n * If this file is present in the sources tree then this is a development SVN\n * version.\n * `date`\n * */\n\n#ifndef __SVNREV_H\n#define __SVNREV_H\n\n$$SVNREV_DEFINE\n\n#endif // __SVNREV_H\n" > svnrev.h

svndist: dist
	tar xzf $(PACKAGE)-$(VERSION).tar.gz; \
	tardir=$(PACKAGE)-$(VERSION)+svn$(REVISION); \
	mv $(PACKAGE)-$(VERSION) $$tardir; \
	cp svnrev.h $$tardir; \
	$(top_srcdir)/config/missing --run tar chof - "$$tardir" | GZIP=--best gzip -c >$(PACKAGE)-$(VERSION)+svn$(REVISION).tar.gz; \
	$(top_srcdir)/config/missing --run tar chof - "$$tardir" | bzip2 -9 -c >$(PACKAGE)-$(VERSION)+svn$(REVISION).tar.bz2; \
	rm $(PACKAGE)-$(VERSION).*; \
	rm -fr "$$tardir";

indent:
	@echo -n "indenting code with Kernighan & Ritchie coding style... "; \
	export VERSION_CONTROL=simple; \
	export SIMPLE_BACKUP_SUFFIX=.ibak; \
	mkdir -p indent-backup && \
	for file in `find -name \*.c -or -name \*.h` ; do \
		indent -kr -i8 -l80 $${file}; \
		cp --parents $${file}$${SIMPLE_BACKUP_SUFFIX} indent-backup && \
		rm $${file}$${SIMPLE_BACKUP_SUFFIX}; \
	done; \
	echo "done."
