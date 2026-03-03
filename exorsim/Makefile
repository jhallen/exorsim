DATADIR ?= /usr/local/share/exorsim/
BINDIR ?= /usr/local/bin/
MANDIR ?= /usr/local/man/man1/

CFLAGS = -g -Wall -DDATADIR=\"$(DATADIR)\"

all : mdos exor unasm6800 edos lpf imdx

unasm6800 : unasm_main.o utils.o unasm.o
	$(CC) -o unasm6800 unasm_main.o utils.o unasm.o

OBJS = utils.o exor.o sim.o asm.o unasm.o exorterm.o mon.o lpt.o drive.o

exor : $(OBJS)
	$(CC) -o exor $(OBJS)
	ln -sf exor exor09
	ln -sf exor exor1
	ln -sf exor swtpc

mdos : mdos.o utils.o
	$(CC) -o mdos mdos.o utils.o

edos : edos.o
	$(CC) -o edos edos.o

imdx : imdx.o
	$(CC) -o imdx imdx.o

lpf : lpf.o
	$(CC) -o lpf lpf.o

.PHONY: clean
clean :
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f mdos.o mdos.d
	rm -f edos.o edos.d
	rm -f lpf.o lpf.d
	rm -f imdx.o imdx.d
	rm -f unasm_main.o unasm_main.d

.PHONY: cleaner
cleaner :
	rm -f exor
	rm -f mdos
	rm -f edos
	rm -f lpf
	rm -f imdx
	rm -f unasm6800

.PHONY: install
install : mdos exor edos unasm6800 lpf imdx
	install -D unasm6800 $(BINDIR)unasm6800
	install -D lpf $(BINDIR)lpf
	install -D imdx $(BINDIR)imdx
	install -D exor $(BINDIR)exor
	ln -sf $(BINDIR)exor $(BINDIR)exor1
	ln -sf $(BINDIR)exor $(BINDIR)swtpc
	ln -sf $(BINDIR)exor $(BINDIR)exor09
	install -D mdos $(BINDIR)mdos
	install -D edos $(BINDIR)edos
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

# include dependancy files if they exist
-include $(OBJS:.o=.d) unasm_main.d mdos.d edos.d imdx.d lpf.d

# compile and generate dependency info
.c.o:
	$(CC) -c $(CFLAGS) -MT $@ -MMD -MP -MF $*.d $< -o $@
