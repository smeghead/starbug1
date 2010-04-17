VERSION = 1.3.12
COPYRIGHT = Copyright\ smeghead\ 2007\ -\ 2010
CC = gcc
CC_VERSION = ${shell gcc --version | grep 'gcc.*[0-9]\.' | sed -e 's/gcc[^0-9]*\([0-9]\).*/\1/g'}

ALT_LIB_PATH = ${HOME}/usr
#ALT_LIB_PATH = /s/usr/MinGW

ifndef INITIAL_LOCALE
	INITIAL_LOCALE = $(basename ${LANG})
endif
ifdef STATIC_LFLAGS
	CFLAGS = -m32 -I${ALT_LIB_PATH}/include -I/usr/include -I/usr/local/include -I. -Idist/cgic205 -Idist/sqlite-3.6.23 -DVERSION=\"${VERSION}\" -DCOPYRIGHT=\"${COPYRIGHT}\" -DINITIAL_LOCALE=\"${INITIAL_LOCALE}\" -O3 -Wall
	LFLAGS = -m32 -L${ALT_LIB_PATH}/lib -L/usr/lib -L/usr/local/lib dist/cgic205/cgic.o dist/sqlite-3.6.23/sqlite3.o
else
	CFLAGS = -m32 -I${ALT_LIB_PATH}/include -I/usr/include -I/usr/local/include -I. -DVERSION=\"${VERSION}\" -DCOPYRIGHT=\"${COPYRIGHT}\" -DINITIAL_LOCALE=\"${INITIAL_LOCALE}\" -O3 -Wall
	LFLAGS = -m32 -L${ALT_LIB_PATH}/lib -L/usr/lib -L/usr/local/lib -lsqlite3 -lcgic
endif

ifeq ($(CC_VERSION), 3)
	CFLAGS += -W
endif
ifeq ($(CC_VERSION), 4)
	CFLAGS += -Wextra
endif
OS = ${shell uname}
ifeq ($(OS), Linux)
	LFLAGS += -ldl -lpthread
endif
ifneq ($(OS), Linux)
	LFLAGS += -lintl -liconv
endif

export INITIAL_LOCALE
#debug
#CFLAGS += -DDEBUG
#CFLAGS += -DMEMORYDEBUG

default: index.cgi admin.cgi compileresource

static: compiledeps
	$(MAKE) STATIC_LFLAGS=yes


admin.o: admin.c alloc.h util.h list.h data.h simple_string.h dbutil.h \
  admin_project.h admin_top.h db_top.h
admin_project.o: admin_project.c data.h list.h simple_string.h \
  db_project.h dbutil.h util.h db_top.h alloc.h wiki.h
admin_top.o: admin_top.c data.h list.h simple_string.h db_top.h dbutil.h \
  util.h db_project.h alloc.h wiki.h hook.h csv.h template.h
alloc.o: alloc.c util.h list.h data.h simple_string.h dbutil.h
conv.o: conv.c simple_string.h
csv.o: csv.c csv.h list.h simple_string.h alloc.h util.h data.h dbutil.h
data.o: data.c data.h list.h simple_string.h alloc.h util.h dbutil.h
db_project.o: db_project.c data.h list.h simple_string.h db_project.h \
  dbutil.h util.h alloc.h
db_top.o: db_top.c data.h list.h simple_string.h db_top.h dbutil.h util.h \
  db_project.h alloc.h
dbutil.o: dbutil.c alloc.h util.h list.h data.h simple_string.h dbutil.h
hook.o: hook.c data.h list.h simple_string.h alloc.h util.h dbutil.h \
  hook.h hook_data.h
index.o: index.c alloc.h util.h list.h data.h simple_string.h dbutil.h \
  index_top.h index_project.h db_top.h
index_project.o: index_project.c data.h list.h simple_string.h \
  db_project.h dbutil.h util.h db_top.h alloc.h conv.h wiki.h hook.h \
  csv.h
index_top.o: index_top.c data.h list.h simple_string.h db_top.h dbutil.h \
  util.h db_project.h alloc.h wiki.h hook.h csv.h
list.o: list.c alloc.h util.h list.h data.h simple_string.h dbutil.h
simple_string.o: simple_string.c alloc.h util.h list.h data.h \
  simple_string.h dbutil.h
util.o: util.c alloc.h conv.h util.h list.h data.h simple_string.h \
  dbutil.h hook.h
wiki.o: wiki.c wiki.h dbutil.h util.h list.h data.h simple_string.h \
  alloc.h db_project.h

index.cgi: list.o simple_string.o data.o dbutil.o db_project.o db_top.o hook.o alloc.o conv.o util.o wiki.o csv.o index_project.o index_top.o index.o
	$(CC) -o $@ $^ $(LFLAGS)
	strip $@

admin.cgi: list.o simple_string.o data.o dbutil.o db_project.o db_top.o alloc.o conv.o util.o wiki.o admin_project.o admin_top.o admin.o template.o
	$(CC) -o $@ $^ $(LFLAGS)
	strip $@

.PHONY: clean webapp dist cvsreleasetag displayinstalldoc mergeresource compileresource compiledeps static
clean:
	rm -f *.o index.cgi admin.cgi
	rm -rf ./dist
	rm -rf ./locale/ja
	rm -rf ./locale/en
	rm -rf ./locale/zh

webapp: default
	@echo "Creating webapp..."
	mkdir -p dist/starbug1
	rsync -a --exclude=*.po --exclude=CVS js css img script locale template *.html *.cgi COPYING_cgic favicon.ico dist/starbug1/
	cp .htaccess dist/starbug1/dot.htaccess
	find dist/starbug1 -name '*.cgi' -exec chmod ugo+x {} \;
	touch dist/starbug1/debug.log
	mkdir -p dist/starbug1/db
	for f in db debug.log; do chmod ugo+w dist/starbug1/$$f; done 
	find dist/starbug1 -name '*.pl' -exec chmod ugo+x {} \;
	@echo "Creating webapp... done."
	@echo "    webapp may be dist/starbug1 directory."

webappwin32: default
	@echo "Creating webapp win32..."
	mkdir -p dist/starbug1-${VERSION}-win32-bin/starbug1
	cp -r js css img script locale template *.html *.cgi COPYING_cgic deps/*.dll favicon.ico dist/starbug1-${VERSION}-win32-bin/starbug1/
	cp .htaccess dist/starbug1-${VERSION}-win32-bin/starbug1/.htaccess
	cp README INSTALL dist/starbug1-${VERSION}-win32-bin/
	@echo "Creating webapp win32... done."
	@echo "    webapp may be dist/starbug1-${VERSION}-win32-bin/starbug1 directory."

dist:
	mkdir -p dist
	cd dist && \
		cvs -d:pserver:anonymous@cvs.sourceforge.jp:/cvsroot/starbug1 export -r HEAD starbug1 && \
		rm -rf starbug1/deps && \
		mv starbug1 starbug1-${VERSION} && \
		tar zcf starbug1-${VERSION}.tar.gz starbug1-${VERSION} && \
		rm -rf starbug1-${VERSION} && \
		cd ..
		@echo "Befor you make new release, You may HAVE TO make tag!"

cvsreleasetag:
	@echo please tag by manual. 
	@echo " " cvs tag starbug1-`echo ${VERSION} | sed 's/\./-/g'`

displayinstalldoc:
	w3m -dump http://starbug1.sourceforge.jp/install

mergeresource:
	xgettext --from-code=utf-8 -k_ --msgid-bugs-address=smeghead@users.sourceforge.jp -L C -p locale  *.c js/*.js
	msgmerge -U locale/ja.po locale/messages.po
	msgmerge -U locale/en.po locale/messages.po
	msgmerge -U locale/zh.po locale/messages.po

compileresource: locale/ja/LC_MESSAGES/starbug1.mo locale/en/LC_MESSAGES/starbug1.mo locale/zh/LC_MESSAGES/starbug1.mo 

locale/ja/LC_MESSAGES/starbug1.mo: locale/ja.po 
	mkdir -p locale/ja/LC_MESSAGES
	msgfmt -o locale/ja/LC_MESSAGES/starbug1.mo locale/ja.po 

locale/en/LC_MESSAGES/starbug1.mo: locale/en.po 
	mkdir -p locale/en/LC_MESSAGES
	msgfmt -o locale/en/LC_MESSAGES/starbug1.mo locale/en.po 

locale/zh/LC_MESSAGES/starbug1.mo: locale/zh.po 
	mkdir -p locale/zh/LC_MESSAGES
	msgfmt -o locale/zh/LC_MESSAGES/starbug1.mo locale/zh.po 

compiledeps:
	mkdir -p dist
	cd dist && \
		wget http://www.boutell.com/cgic/cgic205.tar.gz && \
		tar zxf cgic205.tar.gz && \
		cd cgic205 && \
		make CFLAGS=-m32 LFLAGS=-m32 cgic.o && \
		cd - && \
		wget http://www.sqlite.org/sqlite-amalgamation-3.6.23.tar.gz && \
		tar zxf sqlite-amalgamation-3.6.23.tar.gz && \
		cd sqlite-3.6.23 && \
		./configure && \
		make CFLAGS=-m32 LFLAGS=-m32 sqlite3.o


