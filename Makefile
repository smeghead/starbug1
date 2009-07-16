VERSION = 1.2.10-beta
COPYRIGHT = Copyright\ smeghead\ 2007\ -\ 2009
CC = gcc
CC_VERSION = ${shell gcc --version | grep 'gcc.*[0-9]\.' | sed -e 's/gcc[^0-9]*\([0-9]\).*/\1/g'}
CFLAGS = -I/usr/include -I/usr/local/include -I. -DVERSION=\"${VERSION}\" -DCOPYRIGHT=\"${COPYRIGHT}\" -O3 -Wall
LFLAGS = -L/usr/lib -L/usr/local/lib -lsqlite3 -lcgic
ifeq ($(CC_VERSION), 3)
	CFLAGS += -W
endif
ifeq ($(CC_VERSION), 4)
	CFLAGS += -Wextra
endif
OS = ${shell uname}
ifeq ($(OS), Linux)
	LFLAGS += -ldl
endif
ifeq ($(OS), CYGWIN_NT-5.1)
	LFLAGS += -static
endif

#debug
CFLAGS += -DDEBUG
#CFLAGS += -DMEMORYDEBUG

default: index.cgi admin.cgi


admin.o: admin.c alloc.h util.h list.h data.h simple_string.h dbutil.h \
  admin_project.h admin_top.h db_top.h
admin_project.o: admin_project.c data.h list.h simple_string.h \
  db_project.h dbutil.h util.h db_top.h alloc.h wiki.h
admin_top.o: admin_top.c data.h list.h simple_string.h db_top.h dbutil.h \
  util.h db_project.h alloc.h wiki.h hook.h csv.h
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

admin.cgi: list.o simple_string.o data.o dbutil.o db_project.o db_top.o alloc.o conv.o util.o wiki.o admin_project.o admin_top.o admin.o
	$(CC) -o $@ $^ $(LFLAGS)
	strip $@

.PHONY: clean webapp dist cvsreleasetag displayinstalldoc mergeresource compileresource
clean:
	rm -f *.o index.cgi admin.cgi
	rm -rf ./dist

webapp: default
	@echo "Creating webapp..."
	mkdir -p dist/starbug1
	rsync -a --exclude=*.po --exclude=CVS js css img script locale *.html *.cgi dist/starbug1/
	cp .htaccess dist/starbug1/dot.htaccess
	find dist/starbug1 -name '*.cgi' -exec chmod +x {} \;
	find dist/starbug1 -name '*.pl' -exec chmod +x {} \;
	@echo "Creating webapp... done."
	@echo "    webapp may be dist/starbug1 directory."

dist:
	mkdir -p dist
	cd dist && \
		cvs -d:pserver:anonymous@cvs.sourceforge.jp:/cvsroot/starbug1 export -r HEAD starbug1 && \
		mv starbug1 starbug1-${VERSION} && \
		tar zcf starbug1-${VERSION}.tar.gz starbug1-${VERSION} && \
		rm -rf starbug1-${VERSION} && \
		cd ..
		echo "Befor you make new release, You may HAVE TO make tag!"

cvsreleasetag:
	@echo please tag by manual. 
	@echo " " cvs tag starbug1-`echo ${VERSION} | sed 's/\./-/g'`

displayinstalldoc:
	w3m -dump http://starbug1.sourceforge.jp/install.php

mergeresource:
	xgettext --from-code=utf-8 -k_ --msgid-bugs-address=smeghead@users.sourceforge.jp -L C -p locale  *.c 
	msgmerge -U locale/ja.po locale/messages.po
	msgmerge -U locale/en.po locale/messages.po
	msgmerge -U locale/zh.po locale/messages.po

compileresource:
	mkdir -p locale/ja/LC_MESSAGES
	msgfmt -o locale/ja/LC_MESSAGES/starbug1.mo locale/ja.po 
	mkdir -p locale/en/LC_MESSAGES
	msgfmt -o locale/en/LC_MESSAGES/starbug1.mo locale/en.po 
	mkdir -p locale/zh/LC_MESSAGES
	msgfmt -o locale/zh/LC_MESSAGES/starbug1.mo locale/zh.po 
