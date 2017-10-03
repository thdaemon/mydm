PREFIX :=
INSTALLDIR := /usr/bin

OBJS := mydm.o tools.o su.o xsec.o

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
	cp mydm mydm-greeter-xsec-run $(PREFIX)$(INSTALLDIR)/
	chmod a+x $(PREFIX)$(INSTALLDIR)/mydm $(PREFIX)$(INSTALLDIR)/mydm-greeter-xsec-run

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
