CC = gcc
CFLAGS= -I. -O3 -Wall
LFLAGS = -lsqlite3 -lcgic

default: index.cgi admin.cgi

list.o: list.c list.h
admin.o: admin.c data.h db.h dbutil.h util.h
data.o: data.c data.h util.h dbutil.h
db.o: db.c data.h util.h dbutil.h
dbutil.o: dbutil.c util.h data.h dbutil.h
index.o: index.c data.h db.h dbutil.h util.h wiki.h mail.h
mail.o: mail.c data.h util.h dbutil.h mail.h
util.o: util.c util.h data.h dbutil.h
wiki.o: wiki.c wiki.h util.h data.h dbutil.h

index.cgi: list.o data.o dbutil.o db.o mail.o util.o wiki.o index.o
	$(CC) -o $@ $^ $(LFLAGS)

admin.cgi: list.o data.o dbutil.o db.o util.o css.o admin.o
	$(CC) -o $@ $^ $(LFLAGS)

.PHONY: clean dist
clean:
	rm -f *.o index.cgi admin.cgi
	rm -rf ./dist

webapp: default
	@echo "Creating webapp..."
	mkdir -p dist/starbug1
	rsync -a --exclude CVS js css img wiki script .htaccess *.html *.cgi dist/starbug1/
	find dist/starbug1 -name '*.cgi' -exec chmod +x {} \;
	find dist/starbug1 -name '*.pl' -exec chmod +x {} \;
	@echo "Creating webapp... done."
	@echo "    webapp may be dist/starbug1 directory."

