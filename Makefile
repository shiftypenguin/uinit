CFLAGS=-Wall
all: init shutdown spawn
init: init.c
shutdown: shutdown.c
spawn: spawn.c

clean:
	-rm -f init shutdown spawn

install:
	-install -m 755 init $(DESTDIR)/sbin
	-install -m 755 shutdown $(DESTDIR)/sbin
	-install -m 755 spawn $(DESTDIR)/sbin
	-ln -sf shutdown $(DESTDIR)/sbin/reboot
	-ln -sf shutdown $(DESTDIR)/sbin/halt
	-ln -sf shutdown $(DESTDIR)/sbin/poweroff
