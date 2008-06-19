VERSION = 1.0.1
CC = gcc
CFLAGS = -I/usr/include -I/usr/local/include -I. -DVERSION=\"${VERSION}\" -O3 -Wall
LFLAGS = -L/usr/lib -L/usr/local/lib -lsqlite3 -lcgic
OS = ${shell uname}
ifneq ($(OS), Linux)
	LFLAGS += -liconv
endif
ifeq ($(OS), CYGWIN_NT-5.1)
	LFLAGS += -static
endif

default: index.cgi post.cgi admin.cgi

list.o: list.c list.h
simple_string.o: simple_string.c simple_string.h
admin.o: admin.c data.h db.h dbutil.h util.h
data.o: data.c data.h util.h dbutil.h
db.o: db.c data.h util.h dbutil.h
dbutil.o: dbutil.c util.h data.h dbutil.h
index.o: index.c simple_string.h data.h db.h dbutil.h util.h wiki.h hook.h
hook.o: hook.c data.h util.h dbutil.h hook.h
util.o: util.c util.h data.h dbutil.h
wiki.o: wiki.c wiki.h util.h data.h dbutil.h
csv.o: csv.c csv.h list.h

index.cgi: list.o simple_string.o data.o dbutil.o db.o hook.o util.o wiki.o csv.o index.o
	$(CC) -o $@ $^ $(LFLAGS)
	strip $@

admin.cgi: list.o simple_string.o data.o dbutil.o db.o util.o css.o wiki.o admin.o
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

