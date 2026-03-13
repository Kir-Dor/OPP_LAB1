CFLAGS = -I ./src

mpi_app: main.cpp
	mpicxx $(CFLAGS) -O3 main.cpp -o $@

mpe_app: main.cpp
	mpecxx $(CFLAGS) -O3 -DMPE_BUILD main.cpp -o $@
