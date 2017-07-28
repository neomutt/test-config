CC	?= gcc
RM	= rm -fr
MKDIR	= mkdir -p

DEPDIR	= .dep
OBJDIR	= .obj

OUT	= demo

SRC	+= buffer.c config_set.c data.c extlib.c globals.c hash.c lib.c main.c nntp.c notmuch.c options.c pop.c sidebar.c
SRC	+= type/address.c type/bool.c type/magic.c type/mbyte_table.c type/regex.c type/number.c type/path.c type/quad.c type/sort.c type/string.c
SRC	+= hcache/hcache.c
SRC	+= imap/imap.c
SRC	+= ncrypt/ncrypt.c

OBJ	+= $(SRC:%.c=$(OBJDIR)/%.o)

CFLAGS	+= -Wall
# CFLAGS	+= -Wextra
# CFLAGS	+= -Wno-missing-field-initializers
# CFLAGS	+= -Wno-unused-parameter
CFLAGS	+= -Wpedantic
CFLAGS	+= -g
CFLAGS	+= -I.

LDFLAGS	+= -pthread
LDFLAGS	+= -rdynamic

CFLAGS	+= -fno-omit-frame-pointer

all:	$(OBJDIR) $(DEPDIR) $(OBJ) $(OUT) tags

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

$(DEPDIR) $(OBJDIR):
	$(MKDIR) $@/type $@/ncrypt $@/imap $@/hcache

# ----------------------------------------------------------------------------

tags:	$(SRC) $(HDR)
	ctags -R .

clean:
	$(RM) $(OUT) $(OBJ)

distclean: clean
	$(RM) $(DEPDIR) $(OBJDIR) tags

force:

-include $(SRC:%.c=$(DEPDIR)/%.d)

