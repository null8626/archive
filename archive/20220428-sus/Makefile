CC=gcc
OBJS=$(patsubst src/%.c,src/%.o,$(wildcard src/*.c))
DEL=rm $(OBJS)
DESTDIR=

include deps/tcc/config.mak

CFLAGS=-Wall
tccdirlen=$(shell printf '%s' '$(tccdir)' | wc -c | tr -d '\011\012\013\014\015\040')

.PHONY: clean

clean: sus
	@$(DEL)

sus: $(tccdir)/libtcc1.a $(OBJS)
	$(CC) $(OBJS) -L. -ltcc -ldl -o $@

$(tccdir)/libtcc1.a:
	sudo mkdir -p $(tccdir)
	sudo mv libtcc1.a $(tccdir)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -DSUS_TCC_LIB_FILE=libtcc1.a -DSUS_TCC_RUNTIME_PATH="$(tccdir)" -DSUS_TCC_RUNTIME_PATH_LEN=$(tccdirlen) -c $< -o $@
