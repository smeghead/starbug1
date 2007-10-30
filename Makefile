CC = gcc
CFLAGS= -I. -g -O2 -Wall
LFLAGS = -lsqlite3 -lcgic -static

default: index.cgi admin.cgi

admin.o: admin.c data.h db.h dbutil.h util.h
data.o: data.c data.h util.h dbutil.h
db.o: db.c data.h util.h dbutil.h
dbutil.o: dbutil.c util.h data.h dbutil.h
index.o: index.c data.h db.h dbutil.h util.h wiki.h mail.h
mail.o: mail.c data.h util.h dbutil.h mail.h
util.o: util.c util.h data.h dbutil.h
wiki.o: wiki.c wiki.h util.h data.h dbutil.h

index.cgi: data.o dbutil.o db.o util.o wiki.o mail.o index.o
	$(CC) -o $@ $^ $(LFLAGS)

admin.cgi: data.o dbutil.o db.o util.o css.o admin.o
	$(CC) -o $@ $^ $(LFLAGS)

.PHONY: clean dist
clean:
	rm *.o index.cgi admin.cgi

webapp: default
	mkdir -p dist/starbug1
	cp -r js css img wiki script .htaccess *.cgi dist/starbug1
	find dist/starbug1 -name '*.cgi' -exec chmod +x {} \;
	find dist/starbug1 -name '*.pl' -exec chmod +x {} \;

