CFLAGS = -g -O -Wall -W -pedantic -std=c99 -O0 -I. -I/raylib/src -I../src -DPLATFORM_RPI

LDFLAGS = -L/raylib/src -L/opt/vc/lib
LDLIBS = -lraylib -lm 

boids: src_files/main.c 
	gcc $(CFLAGS) $(LDFLAGS) src_files/main.c -o boids $(LDLIBS)
install: src_files/main.c 
	gcc $(CFLAGS) $(LDFLAGS) src_files/main.c -o boids $(LDLIBS) && cp ./boids $(HOME)/.local/bin/

