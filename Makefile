CC	= gcc
RM	= rm -fr
MKDIR	= mkdir -p

OUT	= demo

SRC	+= debug.c main.c
SRC	+= config/account.c config/config_set.c config/address.c config/bool.c config/magic.c config/mbtable.c config/regex.c config/number.c config/path.c config/quad.c config/sort.c config/string.c
SRC	+= test/common.c test/account.c test/address.c test/bool.c test/configset.c test/initial.c test/magic.c test/mbtable.c test/number.c test/path.c test/quad.c test/regex.c test/sort.c test/string.c test/synonym.c
SRC	+= lib/buffer.c lib/debug.c lib/exit.c lib/hash.c lib/memory.c lib/message.c lib/string.c

OBJ	+= $(SRC:%.c=%.o)

CFLAGS	+= -Wall
# CFLAGS	+= -Wextra
# CFLAGS	+= -Wno-missing-field-initializers
# CFLAGS	+= -Wno-unused-parameter
# CFLAGS	+= -Wno-sign-compare
CFLAGS	+= -Wpedantic
CFLAGS	+= -g
CFLAGS	+= -O0
CFLAGS	+= -I.
CFLAGS	+= -fprofile-arcs -ftest-coverage

CFLAGS	+= -std=c99

LDFLAGS	+= -pthread
LDFLAGS	+= -rdynamic
LDFLAGS	+= -fprofile-arcs -ftest-coverage

CFLAGS	+= -fno-omit-frame-pointer

all:	$(OBJ) $(OUT) tags

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT):	$(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

test:	$(OUT) force
	./$(OUT) configset > test/configset.txt
	./$(OUT) account   > test/account.txt
	./$(OUT) address   > test/address.txt
	./$(OUT) bool      > test/bool.txt
	./$(OUT) initial   > test/initial.txt
	./$(OUT) magic     > test/magic.txt
	./$(OUT) mbtable   > test/mbtable.txt
	./$(OUT) number    > test/number.txt
	./$(OUT) path      > test/path.txt
	./$(OUT) quad      > test/quad.txt
	./$(OUT) regex     > test/regex.txt
	./$(OUT) sort      > test/sort.txt
	./$(OUT) string    > test/string.txt
	./$(OUT) synonym   > test/synonym.txt

tags:	$(SRC) $(HDR)
	ctags -R .

clean:
	$(RM) $(OUT) $(OBJ)

distclean: clean
	$(RM) tags

force:

dummy_dirs:
	(cd config && rm -f config && ln -s . config)
	(cd lib    && rm -f lib    && ln -s . lib)
	(cd test   && rm -f test   && ln -s . test)

coveralls: dummy_dirs all test force
	coveralls -e lib -e test -e debug.c -e debug.h -e main.c

lcov: all test force
	$(RM) lcov
	$(RM) debug.gc?? main.gc?? test/*.gc?? lib/*.gc??
	lcov -t "result" -o lcov.info -c -d config
	genhtml -o lcov lcov.info
	realpath lcov/config/index-sort-l.html

