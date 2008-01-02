"C:/MinGW/bin/gcc.exe" -I/usr/local/include -I. -O3 -Wall   -c -o list.o list.c
"C:/MinGW/bin/gcc.exe" -I/usr/local/include -I. -O3 -Wall   -c -o data.o data.c
"C:/MinGW/bin/gcc.exe" -I/usr/local/include -I. -O3 -Wall   -c -o dbutil.o dbutil.c
"C:/MinGW/bin/gcc.exe" -I/usr/local/include -I. -O3 -Wall   -c -o db.o db.c
"C:/MinGW/bin/gcc.exe" -I/usr/local/include -I. -O3 -Wall   -c -o mail.o mail.c
"C:/MinGW/bin/gcc.exe" -I/usr/local/include -I. -O3 -Wall   -c -o util.o util.c
"C:/MinGW/bin/gcc.exe" -I/usr/local/include -I. -O3 -Wall   -c -o wiki.o wiki.c
"C:/MinGW/bin/gcc.exe" -I/usr/local/include -I. -O3 -Wall   -c -o index.o index.c
"C:/MinGW/bin/gcc.exe" -o index.cgi list.o data.o dbutil.o db.o mail.o util.o wiki.o index.o -L/usr/local/lib -lsqlite3 -lcgic
"C:/MinGW/bin/gcc.exe" -I/usr/local/include -I. -O3 -Wall   -c -o css.o css.c
"C:/MinGW/bin/gcc.exe" -I/usr/local/include -I. -O3 -Wall   -c -o admin.o admin.c
"C:/MinGW/bin/gcc.exe" -o admin.cgi list.o data.o dbutil.o db.o util.o css.o admin.o -L/usr/local/lib -lsqlite3 -lcgic
