# Makefile for libnss-ato

#### Start of system configuration section. ####

CC = gcc
INSTALL = /usr/bin/install
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
DESTDIR = ""

prefix = "/usr"
exec_prefix = ${prefix}

# Where the installed binary goes.
bindir = ${exec_prefix}/bin
binprefix =

sysconfdir = /etc

# mandir = /usr/local/src/less-394/debian/less/usr/share/man
manext = 1
manprefix =

#### End of system configuration section. ####

all:	libnss_ato libnss_ato_test

libnss_ato:	libnss_ato.c
	${CC} ${CFLAGS} ${LDFLAGS} -fPIC -Wall -shared -o libnss_ato.so.2 -Wl,-soname,libnss_ato.so.2 libnss_ato.c

test:	libnss_ato_test.c
	${CC} ${CFLAGS} ${LDFLAGS} -fPIC -Wall -o libnss_ato_test libnss_ato_test.c

install:
	# remeber  /lib/libnss_compat.so.2 -> libnss_compat-2.3.6.so
	${INSTALL_DATA} libnss_ato.so.2 ${DESTDIR}/${prefix}/lib/libnss_ato-2.3.6.so
	${INSTALL_DATA} libnss-ato.3 ${DESTDIR}/${prefix}/share/man/man3
	cd ${DESTDIR}/${prefix}/lib && ln -fs libnss_ato-2.3.6.so libnss_ato.so.2

clean:
	rm -f libnss_ato.so.2 libnss_ato_test
	rm -rf debian/libnss-ato
	rm -f build-stamp
	rm -rf BUILD BUILDROOT RPMS SRPMS SOURCES SPECS

rpm: libnss_ato
	rm -rf BUILD BUILDROOT RPMS SRPMS SOURCES SPECS
	rpmbuild -ba rpm/libnss-ato.spec --define "_topdir $$(pwd)"
