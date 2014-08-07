.if DEBUG
    CFLAGS+=-DDEBUG
    MAKE_FLAGS+=-DDEBUG
.endif

all: whiletrue

whiletrue: $@.c
	${CC} ${CFLAGS} -o $@ $@.c

clean:
	rm -f whiletrue
