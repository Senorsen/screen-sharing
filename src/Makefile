CC=clang
CFLAGS = -std=c99 -O2 -lX11 -lvpx -ljpeg -g
EXEC=scrcast
OBJS=main.o pointer.o imgconv.o vpxenc.o

INSPATH=../

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) 

install:    all
	mkdir -p $(INSPATH) && cp $(EXEC) $(INSPATH)

clean:
	rm -f $(EXEC) *.elf *.o *.gdb *.gcda
