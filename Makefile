CC	?= gcc
RM	= rm -fr
MKDIR	= mkdir -p

DEPDIR	= .dep
OBJDIR	= .obj

OUT	= demo

TYPES	= type/address.c type/bool.c type/magic.c type/mbyte_table.c type/mutt_regex.c type/number.c type/path.c type/quad.c type/sort.c type/string.c

SRC	+= main.c config_set.c lib.c extlib.c hash.c data.c buffer.c hcache.c types.c options.c globals.c
SRC	+= $(TYPES)

OBJ	+= $(SRC:%.c=$(OBJDIR)/%.o)

CFLAGS	+= -Wall
# CFLAGS	+= -Wextra
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
	$(MKDIR) $@/type

# ----------------------------------------------------------------------------

tags:	$(SRC) $(HDR)
	ctags -R .

clean:
	$(RM) $(OUT) $(OBJ)

distclean: clean
	$(RM) $(DEPDIR) $(OBJDIR) tags

force:

-include $(SRC:%.c=$(DEPDIR)/%.d)

