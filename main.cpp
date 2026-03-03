#include "src/SolvePar.hpp"
#include <iostream>
#include <mpi.h>
#include <string>
#include <vector>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, pcount;
  int n = std::stoi(argv[1]);
  int variant = std::stoi(argv[2]);
  double epsilon = 1e-5;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &pcount);

  std::vector<int> rowCounts(pcount, n / pcount);
  for (int i = 0; i < pcount; i++) {
    if (i < n % pcount) {
      rowCounts[i]++;
    }
  }

  std::vector<int> rowIndexes(pcount);
  for (int i = 1; i < pcount; ++i) {
    rowIndexes[i] = rowIndexes[i - 1] + rowCounts[i - 1];
  }

  if (rank == 0) {
    std::cout << "N = " << n << ", variant = " << variant
              << ", pcount = " << pcount << '\n';
  }

  TaskContext ctx;
  ctx.epsilon = epsilon;
  ctx.n = n;
  ctx.pcount = pcount;
  ctx.rank = rank;
  ctx.rowCounts = std::move(rowCounts);
  ctx.rowIndexes = std::move(rowIndexes);

  switch (variant) {
  case 1: {
    solveParVariant1(ctx);
    break;
  }
  case 2: {
    solveParVariant2(ctx);
    break;
  }
  default: {
    std::cout << "Wrong variant!\n";
  }
  }

  MPI_Finalize();
}
