#include "SolvePar.hpp"
#include <iostream>
#include <mpi.h>
#ifdef MPE_BUILD
#include <mpe.h>
#endif
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank, pcount;
    int n = std::stoi(argv[1]);
    int variant = std::stoi(argv[2]);
    double epsilon = 1e-5;

#ifdef MPE_BUILD
    MPE_Init_log();
#endif

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &pcount);

    if (rank == 0)
    {
        std::cout << "Row counts:";
    }
    std::vector<int> rowCounts(pcount, n / pcount);
    for (int i = 0; i < pcount; i++)
    {
        if (i < n % pcount)
        {
            rowCounts[i]++;
        }
        if (rank == 0)
        {
            std::cout << ' ' << rowCounts[i] << ',';
        }
    }
    if (rank == 0)
    {
        std::cout << '\n';
    }

    std::vector<int> rowIndexes(pcount);
    for (int i = 1; i < pcount; ++i)
    {
        rowIndexes[i] = rowIndexes[i - 1] + rowCounts[i - 1];
    }

    if (rank == 0)
    {
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

    switch (variant)
    {
    case 1:
        {
            solveParVariant1(ctx);
            break;
        }
    case 2:
        {
            solveParVariant2(ctx);
            break;
        }
    default:
        {
            std::cout << "Wrong variant!\n";
        }
    }

    char logName[1024];
    sprintf(logName, "mpe_log_v%d.clog2", variant);

#ifdef MPE_BUILD
    MPE_Finish_log(logName);
#endif

    MPI_Finalize();
}
