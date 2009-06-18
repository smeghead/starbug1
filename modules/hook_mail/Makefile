include config

PROGRAM = hook_mail.so
OBJS = hook_mail.o ../../simple_string.o ../../alloc.o
SRCS = $(OBJS:%.o = %.c)
CC = gcc
CFLAGS = -fPIC -Wall -g -DSMTP_SERVER=\"${SMTP_SERVER}\" -DSMTP_PORT=\"${SMTP_PORT}\" -DFROM=\"${FROM}\" -DTO=\"${TO}\"
LDFLAGS= -shared -Wl,-soname,hook_mail.so
LDLIBS=

$(PROGRAM):$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(PROGRAM) $(OBJS) $(LDLIBS)

deploy:
	cp hook_mail.so ../../script/

test:
	gcc -DSMTP_SERVER=\"${SMTP_SERVER}\" -DSMTP_PORT=\"${SMTP_PORT}\" -DFROM=\"${FROM}\" -DTO=\"${TO}\" hook_mail.c \
	 ../../list.o ../../data.o ../../simple_string.o ../../alloc.o ../../conv.o
	./a.out

clean:
	rm -rf $(OBJS)
