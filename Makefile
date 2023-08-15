DATADIR ?= /usr/local/share/exorsim/
BINDIR ?= /usr/local/bin/
MANDIR ?= /usr/local/man/man1/

CFLAGS = -g -Wall

CC = gcc

all : mdos exor exor09 unasm6800 edos lpf imdx

unasm6800 : obj/unasm.o obj/utils.o obj/unasm6800.o
	$(CC) -o unasm6800 obj/unasm.o obj/utils.o obj/unasm6800.o

.PHONY: exor
exor :
	make -f Makefile.00 DATADIR=$(DATADIR)

.PHONY: exor09
exor09 :
	make -f Makefile.09 DATADIR=$(DATADIR)

mdos : obj/mdos.o obj/utils.o
	$(CC) -o mdos obj/mdos.o obj/utils.o

edos : obj/edos.o
	$(CC) -o edos obj/edos.o

imdx : obj/imdx.o
	$(CC) -o imdx obj/imdx.o

lpf : obj/lpf.o
	$(CC) -o lpf obj/lpf.o

.PHONY: clean
clean :
	rm -rf obj
	make -f Makefile.00 clean
	make -f Makefile.09 clean

.PHONY: install
install : mdos exor exor09 edos unasm6800 lpf imdx
	install -D unasm6800 $(BINDIR)unasm6800
	install -D lpf $(BINDIR)lpf
	install -D imdx $(BINDIR)imdx
	install -D exor $(BINDIR)exor
	install -D exor09 $(BINDIR)exor09
	install -D mdos $(BINDIR)mdos
	install -D edos $(BINDIR)edos
	install -D -m 644 facts $(DATADIR)facts
	install -D -m 644 facts09 $(DATADIR)facts09
	install -D -m 644 mdos.dsk $(DATADIR)mdos.dsk
	install -D -m 644 flex.dsk $(DATADIR)flex.dsk
	install -D -m 644 mdos09.dsk $(DATADIR)mdos09.dsk
	install -D -m 644 exbug.bin $(DATADIR)exbug.bin
	install -D -m 644 exbug09.bin $(DATADIR)exbug09.bin
	install -D -m 644 swtbug.bin $(DATADIR)swtbug.bin

# include dependancy files if they exist
-include obj/unasm.d obj/utils.d obj/unasm6800.d obj/mdos.d obj/edos.d obj/imdx.d obj/lpf.d

# compile and generate dependency info
obj/%.o: %.c
	@echo
	@mkdir -p obj/$(shell dirname $*)
	$(CC) -c $(CFLAGS) -MT $@ -MMD -MP -MF obj/$*.d $*.c -o obj/$*.o
