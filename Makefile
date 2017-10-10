PREFIX :=
INSTALLDIR := /usr/bin
DATADIR := /usr/share/mydm

OBJS := mydm.o tools.o su.o xsec.o daemon.o

INSTALL_BINS := mydm mydm-greeter-xsec-run mydm-service-install

CROSS :=
CC := gcc
STRIP := strip

LDFLAGS := -lX11 -L.

ifeq "$(DEBUG)" "1"
	CFLAGS := -Wall -std=c99 -g -DDEBUG=1
	STRIP := ""
else
	CFLAGS := -Wall -O2 -std=c99
endif

all : mydm

mydm : $(OBJS)
	@echo "  LD	$@"
	@$(CROSS)$(CC) -o mydm $(OBJS) $(LDFLAGS)

ifneq "$(DEBUG)" "1"
	@echo "  STRIP	$@"
	@$(CROSS)$(STRIP) -s $@
endif

%.o : %.c config.h
	@echo "  CC	$@"
	@$(CROSS)$(CC) -c -o $@ $(CFLAGS) $<

config.h :
	@echo "Please run ./mkconfig.sh to generate a config header." >&2
	@false

install : all
	mkdir -p $(PREFIX)$(INSTALLDIR)
	cp $(INSTALL_BINS) $(PREFIX)$(INSTALLDIR)/
	chmod a+x $(INSTALL_BINS)
	mkdir -p $(PREFIX)$(DATADIR)
	cp share/* $(PREFIX)$(DATADIR)

bin-tar : all
	./pkgsrc/mkbintar.sh

deb : all
	./pkgsrc/mkdeb.sh

gtk_greeter:
	@$(MAKE) -C greeters/mydm-gtk-demo-greeter

gtk_greeter_install:
	@$(MAKE) -C greeters/mydm-gtk-demo-greeter install

clean:
	rm -f *.o mydm config.h *.so
	rm -rf build-package
	@$(MAKE) -C greeters/mydm-gtk-demo-greeter clean
