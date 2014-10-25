CC=gcc
AR=$(CC)ar
CFLAGS = -std=c99 -O2 -lX11 -ljpeg -g
EXEC=scrshare
OBJS=main.o pointer.o

INSPATH=../

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) 

install:    all
	mkdir -p $(INSPATH) && cp $(EXEC) $(INSPATH)

clean:
	rm -f $(EXEC) *.elf *.o *.gdb *.gcda
