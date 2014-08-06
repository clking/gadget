CC=gcc47
CFLAGS?=
LIB=libfaketime
TEST=faketest
LINKING_LIB?=-lm
MAKE_FLAGS=

.if DEBUG
    CFLAGS+=-DDEBUG
    MAKE_FLAGS+=-DDEBUG
.endif

.if INFO
    CFLAGS+=-DINFO
    MAKE_FLAGS+=-DINFO
.endif

.if WARN
    CFLAGS+=-DWARN
    MAKE_FLAGS+=-DWARN
.endif

.if STRICT
    CFLAGS+=-Wall -pedantic
    MAKE_FLAGS=-DSTRICT
.endif

all: ${LIB}

test: ${TEST}

libfaketime: libfaketime.c
	${CC} ${CFLAGS} ${LINKING_LIB} -shared -export-dynamic -fPIC -nostdlib -o $@.so $@.c

faketest: libfaketime faketimetest runtest

faketimetest: libfaketime
	cd t ; \
	make ${MAKE_FLAGS} $@

runtest: libfaketime faketimetest
	t/test.sh

perltest: libfaketime
	env LD_PRELOAD="./libfaketime.so" FAKE_TIME=1209600 ./faketimetest.pl

clean:
	rm -fv *~ *.core *.tmp *.so *test ;\
	cd t ; \
	make ${MAKE_FLAGS} $@
