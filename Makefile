CFLAGS = -g -Wall

CC = gcc

all : mdos exor exor09 unasm edos lpf imdx

unasm : obj/unasm.o obj/utils.o obj/unasm6800.o
	$(CC) -o unasm obj/unasm.o obj/utils.o obj/unasm6800.o

.PHONY: exor
exor :
	make -f Makefile.00

.PHONY: exor09
exor09 :
	make -f Makefile.09

mdos : obj/mdos.o
	$(CC) -o mdos obj/mdos.o

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

# include dependancy files if they exist
-include obj/unasm.d obj/utils.d obj/unasm6800.d obj/mdos.d obj/edos.d obj/imdx.d obj/lpf.d

# compile and generate dependency info
obj/%.o: %.c
	@echo
	@mkdir -p obj/$(shell dirname $*)
	$(CC) -c $(CFLAGS) -MT $@ -MMD -MP -MF obj/$*.d $*.c -o obj/$*.o
