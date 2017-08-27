CC	= gcc
RM	= rm -fr
MKDIR	= mkdir -p

OUT	= demo

SRC	+= account.c config_set.c debug.c main.c
SRC	+= config/address.c config/bool.c config/magic.c config/mbyte_table.c config/regex.c config/number.c config/path.c config/quad.c config/sort.c config/string.c
SRC	+= test/common.c test/address.c test/bool.c test/configset.c test/initial.c test/magic.c test/mbyte_table.c test/number.c test/path.c test/quad.c test/regex.c test/sort.c test/string.c test/synonym.c
SRC	+= lib/buffer.c lib/debug.c lib/exit.c lib/hash.c lib/memory.c lib/message.c lib/string.c

OBJ	+= $(SRC:%.c=%.o)

CFLAGS	+= -Wall
# CFLAGS	+= -Wextra
# CFLAGS	+= -Wno-missing-field-initializers
# CFLAGS	+= -Wno-unused-parameter
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
	./$(OUT) configset   > test/configset.txt
	./$(OUT) address     > test/address.txt
	./$(OUT) bool        > test/bool.txt
	./$(OUT) initial     > test/initial.txt
	./$(OUT) magic       > test/magic.txt
	./$(OUT) mbyte_table > test/mbyte_table.txt
	./$(OUT) number      > test/number.txt
	./$(OUT) path        > test/path.txt
	./$(OUT) quad        > test/quad.txt
	./$(OUT) regex       > test/regex.txt
	./$(OUT) sort        > test/sort.txt
	./$(OUT) string      > test/string.txt
	./$(OUT) synonym     > test/synonym.txt

tags:	$(SRC) $(HDR)
	ctags -R .

covclean:
	$(RM) *.gc?? */*.gc??

clean: covclean
	$(RM) $(OUT) $(OBJ)

distclean: clean
	$(RM) tags

force:

dummy_dirs:
	(cd config && rm -f config && ln -s . config)
	(cd lib    && rm -f lib    && ln -s . lib)
	(cd test   && rm -f test   && ln -s . test)

coveralls: dummy_dirs
	coveralls -e lib -e test -e debug.c -e debug.h -e main.c

