PREFIX :=
INSTALLDIR := /usr/lib/mydm/mydm-gtk-demo-greeter

OBJS := mydm-gtk-demo-greeter.o ../../su.o

CROSS :=
CC := gcc
STRIP := strip

CFLAGS := -Wall -O2 -std=c99 `pkg-config --cflags gtk+-3.0`
LDFLAGS := `pkg-config --libs gtk+-3.0` -lcrypt -rdynamic

all : mydm-gtk-demo-greeter

mydm-gtk-demo-greeter : $(OBJS)
	$(CROSS)$(CC) -o $@ $(OBJS) $(LDFLAGS)
	$(CROSS)$(STRIP) -s $@

mydm-gtk-demo-greeter.o : mydm-gtk-demo-greeter.c
	$(CROSS)$(CC) -c -o $@ $(CFLAGS) mydm-gtk-demo-greeter.c -lcrypt

../../su.o :
	@echo ' >>> You need compile mydm first, because I need it!'
	@false

clean:
	rm -f *.o mydm-gtk-demo-greeter

install : all
	mkdir -p $(PREFIX)$(INSTALLDIR)/
	cp mydm-gtk-demo-greeter deinit mydm-gtk-demo-greeter.glade $(PREFIX)$(INSTALLDIR)/
	chmod +x $(PREFIX)$(INSTALLDIR)/mydm-gtk-demo-greeter deinit
