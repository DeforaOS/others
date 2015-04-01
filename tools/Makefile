MKDIR	= mkdir -m 0755 -p
INSTALL	= install
RM	= rm -f
TARGETS	= $(OBJDIR)linuxrc $(OBJDIR)mksparse $(OBJDIR)oinit $(OBJDIR)otherbox $(OBJDIR)others.c
OBJDIR	=
PREFIX	= /usr/local
DESTDIR	=
BINDIR	= $(PREFIX)/bin
SBINDIR	= $(PREFIX)/sbin
CC	= cc
CPPFLAGSF=
CPPFLAGS=
CFLAGSF	= -W
CFLAGS	= -Wall -g -O2 -pedantic
RM	= rm -f
LN	= ln -f
MKDIR	= mkdir -m 0755 -p
INSTALL	= install


all: $(TARGETS)

linuxrc_OBJS = $(OBJDIR)linuxrc.o
linuxrc_CFLAGS = $(CPPFLAGSF) $(CPPFLAGS) $(CFLAGSF) $(CFLAGS)
linuxrc_LDFLAGS = $(LDFLAGSF) $(LDFLAGS)

$(OBJDIR)linuxrc: $(linuxrc_OBJS)
	$(CC) -o $(OBJDIR)linuxrc $(linuxrc_OBJS) $(linuxrc_LDFLAGS)

mksparse_OBJS = $(OBJDIR)mksparse.o
mksparse_CFLAGS = $(CPPFLAGSF) $(CPPFLAGS) $(CFLAGSF) $(CFLAGS)
mksparse_LDFLAGS = $(LDFLAGSF) $(LDFLAGS)

$(OBJDIR)mksparse: $(mksparse_OBJS)
	$(CC) -o $(OBJDIR)mksparse $(mksparse_OBJS) $(mksparse_LDFLAGS)

oinit_OBJS = $(OBJDIR)oinit.o
oinit_CFLAGS = $(CPPFLAGSF) $(CPPFLAGS) $(CFLAGSF) $(CFLAGS)
oinit_LDFLAGS = $(LDFLAGSF) $(LDFLAGS)

$(OBJDIR)oinit: $(oinit_OBJS)
	$(CC) -o $(OBJDIR)oinit $(oinit_OBJS) $(oinit_LDFLAGS)

otherbox_OBJS = $(OBJDIR)otherbox.o $(OBJDIR)others.o
otherbox_CFLAGS = $(CPPFLAGSF) $(CPPFLAGS) $(CFLAGSF) $(CFLAGS)
otherbox_LDFLAGS = $(LDFLAGSF) $(LDFLAGS) 

$(OBJDIR)otherbox: $(otherbox_OBJS)
	$(CC) -o $(OBJDIR)otherbox $(otherbox_OBJS) $(otherbox_LDFLAGS)

$(OBJDIR)others.c: toolbox.sh
	./toolbox.sh -P "$(PREFIX)" -- "$(OBJDIR)others.c"

$(OBJDIR)linuxrc.o: linuxrc.c
	$(CC) $(linuxrc_CFLAGS) -o $(OBJDIR)linuxrc.o -c linuxrc.c

$(OBJDIR)mksparse.o: mksparse.c
	$(CC) $(mksparse_CFLAGS) -o $(OBJDIR)mksparse.o -c mksparse.c

$(OBJDIR)oinit.o: oinit.c
	$(CC) $(oinit_CFLAGS) -o $(OBJDIR)oinit.o -c oinit.c

$(OBJDIR)otherbox.o: otherbox.c otherbox.h
	$(CC) $(otherbox_CFLAGS) -o $(OBJDIR)otherbox.o -c otherbox.c

$(OBJDIR)others.o: others.c toolbox.sh
	$(CC) $(otherbox_CFLAGS) -o $(OBJDIR)others.o -c others.c

clean:
	$(RM) -- $(linuxrc_OBJS) $(mksparse_OBJS) $(oinit_OBJS) $(otherbox_OBJS) $(others.c_OBJS)
	./toolbox.sh -c -P "$(PREFIX)" -- "others.c"

distclean: clean
	$(RM) -- $(TARGETS)

install: $(TARGETS)
	$(MKDIR) $(DESTDIR)$(BINDIR)
	$(INSTALL) -m 0755 watch.sh $(DESTDIR)$(BINDIR)/watch.sh

uninstall:
	$(RM) -- $(DESTDIR)$(BINDIR)/watch.sh

.PHONY: all clean distclean install uninstall
