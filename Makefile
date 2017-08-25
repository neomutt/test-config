CC	= gcc
RM	= rm -fr
MKDIR	= mkdir -p

DEPDIR	= .dep
DEPALL	= $(DEPDIR)/type $(DEPDIR)/ncrypt $(DEPDIR)/imap $(DEPDIR)/hcache $(DEPDIR)/lib $(DEPDIR)/test
OBJDIR	= .obj
OBJALL	= $(OBJDIR)/type $(OBJDIR)/ncrypt $(OBJDIR)/imap $(OBJDIR)/hcache $(OBJDIR)/lib $(OBJDIR)/test

OUT	= demo

SRC	+= account.c config_set.c debug.c globals.c main.c nntp.c notmuch.c options.c pop.c sidebar.c validate.c
SRC	+= type/address.c type/bool.c type/magic.c type/mbyte_table.c type/regex.c type/number.c type/path.c type/quad.c type/sort.c type/string.c
SRC	+= hcache/hcache.c
SRC	+= imap/imap.c
SRC	+= ncrypt/ncrypt.c
SRC	+= test/common.c test/address.c test/bool.c test/initial.c test/magic.c test/mbyte_table.c test/number.c test/path.c test/quad.c test/regex.c test/sort.c test/string.c test/synonym.c
SRC	+= lib/base64.c lib/buffer.c lib/date.c lib/debug.c lib/exit.c lib/hash.c lib/memory.c lib/message.c lib/sha1.c lib/string.c

OBJ	+= $(SRC:%.c=$(OBJDIR)/%.o)

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

all:	$(OBJALL) $(DEPALL) $(OBJ) $(OUT) tags

# ----------------------------------------------------------------------------

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ && ( \
	$(CC) -MM $(CFLAGS) -c $< | sed 's/.*:/'$(OBJDIR)'\/\0/' > $(DEPDIR)/$*.d; \
	cp -f $(DEPDIR)/$*.d $(DEPDIR)/$*.d.tmp; \
	sed -e 's/.*://' -e 's/\\$$//' < $(DEPDIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(DEPDIR)/$*.d; \
	rm -f $(DEPDIR)/$*.d.tmp)

# ----------------------------------------------------------------------------

$(OUT):	$(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

$(DEPALL) $(OBJALL):
	$(MKDIR) $@

# ----------------------------------------------------------------------------

test:	$(OBJALL) $(DEPALL) $(OUT) force
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

clean:
	$(RM) $(OUT) $(OBJ)

distclean: clean
	$(RM) $(DEPDIR) $(OBJDIR) tags

force:

-include $(SRC:%.c=$(DEPDIR)/%.d)

