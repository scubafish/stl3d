CC	= gcc
CFLAGS	= -lm
SRC	= maintest.c stl3d_lib.c

maintest: $(SRC) stl3d_lib.h
	$(CC) $(CFLAGS) -o maintest $(SRC)

clean:
	rm maintest
