PROGRAM = hook_mail
OBJS = hook_mail.o
SRCS = $(OBJS:%.o = %.c)
CC = gcc
CFLAGS = -Wall -g
LDFLAGS=
LDLIBS=
$(PROGRAM):$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(PROGRAM) $(OBJS) $(LDLIBS)

clean:
	rm -rf $(OBJS)
