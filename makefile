CC = g++
CFLAGS = -O3

default:
	$(MAKE) --directory=src/
	mv src/barnes_algorithm.exe       ./
	mv src/garbrecht_algorithm.exe    ./
	mv src/generate_square_grid.exe   ./
