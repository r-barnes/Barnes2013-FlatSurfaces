CC = g++
CFLAGS = -O3

default:
	$(MAKE) --directory=src/
	mv src/barnes_algorithm.exe       ./
	mv src/garbrecht_algorithm.exe    ./
	mv src/generate_square_grid.exe   ./

clean:
	rm -f barnes_algorithm.exe garbrecht_algorithm.exe generate_square_grid.exe
