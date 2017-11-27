
prefix = /usr
installdir = $(prefix)/lib/pd-externals/jacktime
installfiles = COPYING README.md Makefile jacktime.pd_lua jacktime-help.pd jtime.c jtime.so

all: jtime.so

jtime.so: jtime.c
	$(CC) -shared -fPIC -o $@ $< $(shell pkg-config --cflags --libs jack) $(shell pkg-config --cflags --libs lua)

clean:
	rm -f jtime.so

install:
	mkdir -p $(DESTDIR)$(installdir)
	cp $(installfiles) $(DESTDIR)$(installdir)

uninstall:
	rm -rf $(DESTDIR)$(installdir)
