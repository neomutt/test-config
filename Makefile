CC	= gcc
# CC	= clang
RM	= rm -fr
MKDIR	= mkdir -p

OUT	= demo

SRC	+= main.c
SRC	+= config/account.c config/address.c config/bool.c config/dump.c config/enum.c config/magic.c config/mbtable.c config/regex.c config/number.c config/path.c config/quad.c config/set.c config/slist.c config/sort.c config/string.c
SRC	+= test/common.c test/account.c test/address.c test/bool.c test/enum.c test/initial.c test/magic.c test/mbtable.c test/number.c test/path.c test/quad.c test/regex.c test/set.c test/slist.c test/sort.c test/string.c test/synonym.c
SRC	+= mutt/address.c mutt/base64.c mutt/buffer.c mutt/charset.c mutt/date.c mutt/envlist.c mutt/exit.c mutt/file.c mutt/hash.c mutt/idna.c mutt/list.c mutt/logging.c mutt/mapping.c mutt/mbyte.c mutt/md5.c mutt/memory.c mutt/mime.c mutt/parameter.c mutt/regex.c mutt/rfc2047.c mutt/sha1.c mutt/signal.c mutt/string.c
SRC	+= dump/dump.c dump/data.c dump/vars.c

OBJ	+= $(SRC:%.c=%.o)

CFLAGS	+= -Wall
# CFLAGS	+= -Wextra
# CFLAGS	+= -Wno-missing-field-initializers
# CFLAGS	+= -Wno-unused-parameter
# CFLAGS	+= -Wno-sign-compare
CFLAGS	+= -Wpedantic
CFLAGS	+= -ggdb3
CFLAGS	+= -O0
CFLAGS	+= -I.
CFLAGS	+= -fprofile-arcs -ftest-coverage
# CFLAGS	+= -fsanitize=address -fsanitize-recover=address

CFLAGS	+= -std=c99

LDFLAGS	+= -lidn
LDFLAGS	+= -pthread
LDFLAGS	+= -rdynamic
LDFLAGS	+= -fprofile-arcs -ftest-coverage
# LDFLAGS	+= -fsanitize=address -fsanitize-recover=address

CFLAGS	+= -fno-omit-frame-pointer

all:	$(OBJ) $(OUT) tags

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT):	$(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

test:	$(OUT) force
	-./$(OUT) set     > test/set.txt
	-./$(OUT) account > test/account.txt
	-./$(OUT) initial > test/initial.txt
	-./$(OUT) synonym > test/synonym.txt
	-./$(OUT) address > test/address.txt
	-./$(OUT) bool    > test/bool.txt
	-./$(OUT) enum    > test/enum.txt
	-./$(OUT) magic   > test/magic.txt
	-./$(OUT) mbtable > test/mbtable.txt
	-./$(OUT) number  > test/number.txt
	-./$(OUT) path    > test/path.txt
	-./$(OUT) quad    > test/quad.txt
	-./$(OUT) regex   > test/regex.txt
	-./$(OUT) slist   > test/slist.txt
	-./$(OUT) sort    > test/sort.txt
	-./$(OUT) string  > test/string.txt
	-./$(OUT) dump    > dump/dump.txt

tags:	$(SRC) $(HDR) force
	ctags -R .

clean:
	$(RM) $(OUT) $(OBJ)

distclean: clean
	$(RM) tags
	$(RM) *.gc?? */*.gc??
	$(RM) lcov.info lcov

force:

dummy_dirs:
	(cd config && rm -f config && ln -s . config)
	(cd mutt   && rm -f mutt   && ln -s . mutt)
	(cd test   && rm -f test   && ln -s . test)

coveralls: dummy_dirs all test force
	coveralls -e mutt -e test -e dump -e main.c -e config/dump.c

lcov: all test force
	$(RM) lcov
	$(RM) main.gc?? test/*.gc?? mutt/*.gc??
	lcov -t "result" -o lcov.info -c -d config
	genhtml -o lcov lcov.info
	realpath lcov/config/index-sort-l.html

