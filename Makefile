PREFIX :=
INSTALLDIR := /opt

OBJS := mydm.o tools.o

CROSS :=
CC := gcc
STRIP := strip

CFLAGS := -Wall -O2 -std=c99
LDFLAGS := -lX11

all : mydm

mydm : $(OBJS)
	@echo "  LD	$@"
	@$(CROSS)$(CC) -o mydm $(OBJS) $(LDFLAGS)

	@echo "  STRIP	$@"
	@$(CROSS)$(STRIP) -s $@

%.o : %.c
	@echo "  CC	$@"
	@$(CROSS)$(CC) -c -o $@ $(CFLAG) $<

install : all
	cp mydm $(INSTALLDIR)/mydm

clean:
	rm -f *.o mydm
