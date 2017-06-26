PREFIX :=
INSTALLDIR := /usr/local/bin

OBJS := mydm.o tools.o su.o xsec.o

CROSS :=
CC := gcc
STRIP := strip

CFLAGS := -Wall -O2 -std=c99
LDFLAGS := -lX11 -L.


all : mydm

mydm : $(OBJS)
	@echo "  LD	$@"
	@$(CROSS)$(CC) -o mydm $(OBJS) $(LDFLAGS)

	@echo "  STRIP	$@"
	@$(CROSS)$(STRIP) -s $@

%.o : %.c config.h
	@echo "  CC	$@"
	@$(CROSS)$(CC) -c -o $@ $(CFLAGS) $<

config.h :
	@echo "Please run ./mkconfig.sh to generate a config header." >&2
	@false

install : all
	cp mydm $(PREFIX)$(INSTALLDIR)/mydm

bin-tar : all
	./pkgsrc/mkbintar.sh

clean:
	rm -f *.o mydm config.h *.so
	rm -rf build-package
