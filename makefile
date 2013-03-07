CC = g++
CFLAGS = -O3

default:
	$(MAKE) --directory=src/
	mv src/barnes_algorithm ./
	mv src/garbrecht_algorithm    ./
	mv src/generate_square_grid   ./
