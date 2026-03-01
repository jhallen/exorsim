DATADIR ?= /usr/local/share/exorsim/
BINDIR ?= /usr/local/bin/
MANDIR ?= /usr/local/man/man1/

CFLAGS = -g -Wall -DDATADIR=\"$(DATADIR)\"

BIN_MDOS = mdos
OBJS_MDOS = mdos.o utils.o

BIN_UNASM6800 = unasm6800
OBJS_UNASM6800 = unasm.o utils.o unasm6800.o

BIN_EDOS = edos
OBJS_EDOS = edos.o

BIN_LPF = lpf
OBJS_LPF = lpf.o

BIN_IMDX = imdx
OBJS_IMDX = imdx.o

BINS	=	$(BIN_MDOS) \
		exor \
		exor09 \
		$(BIN_UNASM6800) \
		$(BIN_EDOS) \
		$(BIN_LPF) \
		$(BIN_IMDX)

OBJS	=	$(OBJS_MDOS) \
		$(OBJS_UNASM6800) \
		$(OBJS_EDOS) \
		$(OBJS_LPF) \
		$(OBJS_IMDX)

all : $(BINS)

$(BIN_UNASM6800) : $(OBJS_UNASM6800)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN_UNASM6800) $(OBJS_UNASM6800)

.PHONY: exor
exor :
	make -f Makefile.00 DATADIR=$(DATADIR)

.PHONY: exor09
exor09 :
	make -f Makefile.09 DATADIR=$(DATADIR)

$(BIN_MDOS) : $(OBJS_MDOS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN_MDOS) $(OBJS_MDOS)

$(BIN_EDOS) : $(OBJS_EDOS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN_EDOS) $(OBJS_EDOS)

$(BIN_IMDX) : $(OBJS_IMDX)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN_IMDX) $(OBJS_IMDX)

$(BIN_LPF) : $(OBJS_LPF)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN_LPF) $(OBJS_LPF)

.PHONY: clean
clean :
	rm -rf $(OBJS) $(BINS)
	make -f Makefile.00 clean
	make -f Makefile.09 clean

.PHONY: install
install : $(BINS)
	install -D $(BIN_UNASM6800) $(BINDIR)$(BIN_UNASM6800)
	install -D $(BIN_LPF) $(BINDIR)$(BIN_LPF)
	install -D $(BIN_IMDX) $(BINDIR)$(BIN_IMDX)
	install -D exor $(BINDIR)exor
	install -D exor $(BINDIR)exor1
	install -D exor $(BINDIR)swtpc
	install -D exor09 $(BINDIR)exor09
	install -D $(BIN_MDOS) $(BINDIR)$(BIN_MDOS)
	install -D $(BIN_EDOS) $(BINDIR)$(BIN_EDOS)
	install -D -m 644 facts $(DATADIR)facts
	install -D -m 644 facts09 $(DATADIR)facts09
	install -D -m 644 mdos.dsk $(DATADIR)mdos.dsk
	install -D -m 644 edos.dsk $(DATADIR)edos.dsk
	install -D -m 644 edos_blank.dsk $(DATADIR)edos_blank.dsk
	install -D -m 644 flex.dsk $(DATADIR)flex.dsk
	install -D -m 644 mdos09.dsk $(DATADIR)mdos09.dsk
	install -D -m 644 exbug.bin $(DATADIR)exbug.bin
	install -D -m 644 exordisk.s19 $(DATADIR)exordisk.s19
	install -D -m 644 exbug09.bin $(DATADIR)exbug09.bin
	install -D -m 644 swtbug.bin $(DATADIR)swtbug.bin
	install -D -m 644 exor.setup $(DATADIR)exor.setup
	install -D -m 644 exor1.setup $(DATADIR)exor1.setup
	install -D -m 644 exor09.setup $(DATADIR)exor09.setup
	install -D -m 644 swtpc.setup $(DATADIR)swtpc.setup
