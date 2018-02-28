CC	= gcc
CFLAGS	= -Wall -lm
SRC	= maintest.c stl3d_lib.c stl3d_readwrite.c stl3d_heightmap.c

maintest: $(SRC) stl3d_lib.h
	$(CC) $(CFLAGS) -o maintest $(SRC)

clean:
	rm maintest
