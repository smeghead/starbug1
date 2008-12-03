VERSION = 1.2.3-alpha
CC = gcc
CC_VERSION = ${shell gcc --version | grep 'gcc.*[0-9]\.' | sed -e 's/gcc[^0-9]*\([0-9]\).*/\1/g'}
CFLAGS = -I/usr/include -I/usr/local/include -I. -DVERSION=\"${VERSION}\" -O3 -Wall
LFLAGS = -L/usr/lib -L/usr/local/lib -lsqlite3 -lcgic
ifeq ($(CC_VERSION), 3)
	CFLAGS += -W
endif
ifeq ($(CC_VERSION), 4)
	CFLAGS += -Wextra
endif
OS = ${shell uname}
ifneq ($(OS), Linux)
	LFLAGS += -liconv
endif
ifeq ($(OS), CYGWIN_NT-5.1)
	LFLAGS += -static
endif

#debug
CFLAGS += -DDEBUG

default: index.cgi admin.cgi

admin.o: admin.c util.h list.h data.h dbutil.h simple_string.h admin_project.h
admin_project.o: admin_project.c data.h list.h db_project.h dbutil.h util.h simple_string.h db_top.h wiki.h
admin_top.o: admin_top.c data.h list.h db_project.h dbutil.h util.h simple_string.h db_top.h wiki.h
csv.o: csv.c csv.h list.h simple_string.h util.h data.h dbutil.h
data.o: data.c data.h list.h util.h dbutil.h simple_string.h
db_project.o: db_project.c data.h list.h db_project.h dbutil.h util.h simple_string.h
db_top.o: db_top.c data.h list.h db_project.h dbutil.h util.h simple_string.h
dbutil.o: dbutil.c util.h list.h data.h dbutil.h simple_string.h
hook.o: hook.c data.h list.h util.h dbutil.h simple_string.h hook.h
index.o: index.c util.h list.h data.h dbutil.h simple_string.h index_project.h
index_project.o: index_project.c data.h list.h db_project.h dbutil.h util.h simple_string.h db_top.h wiki.h hook.h csv.h
index_top.o: index_top.c data.h list.h db_top.h dbutil.h util.h simple_string.h wiki.h hook.h csv.h
list.o: list.c util.h list.h data.h dbutil.h simple_string.h
simple_string.o: simple_string.c data.h list.h util.h dbutil.h simple_string.h
util.o: util.c util.h list.h data.h dbutil.h simple_string.h hook.h
wiki.o: wiki.c wiki.h dbutil.h util.h list.h data.h simple_string.h db_project.h

index.cgi: list.o simple_string.o data.o dbutil.o db_project.o db_top.o hook.o util.o wiki.o csv.o index_project.o index_top.o index.o
	$(CC) -o $@ $^ $(LFLAGS)
	strip $@

admin.cgi: list.o simple_string.o data.o dbutil.o db_project.o db_top.o util.o wiki.o admin_project.o admin_top.o admin.o
	$(CC) -o $@ $^ $(LFLAGS)
	strip $@

.PHONY: clean webapp dist cvsreleasetag displayinstalldoc
clean:
	rm -f *.o index.cgi admin.cgi
	rm -rf ./dist

webapp: default
	@echo "Creating webapp..."
	mkdir -p dist/starbug1
	rsync -a --exclude=CVS js css img script *.html *.cgi dist/starbug1/
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

cvsreleasetag:
	@echo please tag by manual. 
	@echo " " cvs tag starbug1-`echo ${VERSION} | sed 's/\./-/g'`

displayinstalldoc:
	w3m -dump http://starbug1.sourceforge.jp/install.php

