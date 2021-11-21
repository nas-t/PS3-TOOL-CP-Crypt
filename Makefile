TOOLS	=	cp_crypt
COMMON	=	tools.o aes.o md5.o
DEPS	=	Makefile tools.h types.h

CC	=	gcc
CFLAGS	=	-g -O3 -W
LDFLAGS =	

OBJS	= $(COMMON) $(addsuffix .o, $(TOOLS))

all: $(TOOLS)

$(TOOLS): %: %.o $(COMMON) $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LDFLAGS)

$(OBJS): %.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f $(OBJS) $(TOOLS)
