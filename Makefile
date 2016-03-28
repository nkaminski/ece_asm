CC = gcc
FILES = asm.c
OUT_EXE = asm

build: $(FILES)
	$(CC) -o $(OUT_EXE) $(FILES)

clean:
	rm -f *.o core asm

