#pragma once

#include "CheckAnswer.hpp"
#include "Generate.hpp"
#include "MathOp.hpp"
#include "Structs.hpp"
#include <mpi.h>
#include <cmath>
#include <vector>

struct TaskContext
{
    std::vector<int> rowCounts, rowIndexes;
    double epsilon;
    int n, rank, pcount;
};

inline void solveParVariant1(const TaskContext& ctx)
{
    MathStructs::Matrix localMatrixA = generateLocalMatrix(
        ctx.n, ctx.rowIndexes[ctx.rank], ctx.rowCounts[ctx.rank]);

    MathStructs::Vector vectorB = generateLocalVector(ctx.n, 0, ctx.n);
    MathStructs::Vector vectorX(ctx.n, 0.0);

    auto timeStart = MPI_Wtime();

    MathStructs::Vector vectorR = vectorB;
    MathStructs::Vector vectorZ = vectorR;
    MathStructs::Vector vectorTemp(ctx.n);
    MathStructs::Vector globalVectorAZ(ctx.n);
    MathStructs::Vector localVectorAZ(ctx.rowCounts[ctx.rank]);
    MathStructs::Vector vectorRNext(ctx.n);

    while (lenVec(vectorR) / lenVec(vectorB) > ctx.epsilon)
    {
        multiply(localMatrixA, vectorZ, localVectorAZ, 0);
        MPI_Allgatherv(localVectorAZ.data(), static_cast<int>(localVectorAZ.size()), MPI_DOUBLE,
                       globalVectorAZ.data(), ctx.rowCounts.data(),
                       ctx.rowIndexes.data(), MPI_DOUBLE, MPI_COMM_WORLD);

        double scalarA =
            dotVecVec(vectorR, vectorR) / dotVecVec(globalVectorAZ, vectorZ);

        multiply(scalarA, vectorZ, vectorTemp);
        add(vectorX, vectorTemp, vectorX);

        multiply(scalarA, globalVectorAZ, vectorTemp);
        substract(vectorR, vectorTemp, vectorRNext);

        double scalarB =
            dotVecVec(vectorRNext, vectorRNext) / dotVecVec(vectorR, vectorR);

        multiply(scalarB, vectorZ, vectorTemp);
        add(vectorRNext, vectorTemp, vectorZ);

        std::swap(vectorR, vectorRNext);
    }

    auto duration = MPI_Wtime() - timeStart;

    if (ctx.rank == 0)
    {
        MathStructs::Matrix matrixA = generateLocalMatrix(ctx.n, 0, ctx.n);
        std::cout << "Time spent: " << duration << " seconds\n";
        std::cout << "inaccuracy: " << checkAnswer(matrixA, vectorB, vectorX)
            << '\n';
    }
}

inline void solveParVariant2(const TaskContext& ctx)
{
    MathStructs::Matrix localMatrixA = generateLocalMatrix(
        ctx.n, ctx.rowIndexes[ctx.rank], ctx.rowCounts[ctx.rank]);

    MathStructs::Vector localVectorB = generateLocalVector(ctx.n, ctx.rowIndexes[ctx.rank], ctx.rowCounts[ctx.rank]);
    MathStructs::Vector localVectorX(ctx.rowCounts[ctx.rank], 0.0);

    auto timeStart = MPI_Wtime();

    double localVectorBProduct = dotVecVec(localVectorB, localVectorB);
    double globalVectorBProduct;
    MPI_Allreduce(&localVectorBProduct, &globalVectorBProduct, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    double globalVectorRProduct = globalVectorBProduct;

    MathStructs::Vector localVectorR = localVectorB;
    MathStructs::Vector localVectorZ = localVectorR;
    MathStructs::Vector localVectorRNext(ctx.rowCounts[ctx.rank]);

    MathStructs::Vector localVectorAZ(ctx.rowCounts[ctx.rank]);

    MathStructs::Vector localVectorTemp(ctx.rowCounts[ctx.rank]);

    MathStructs::Vector otherVectorZ(ctx.rowCounts[0]);

    std::vector<MPI_Request> requests(ctx.pcount);

    while (std::sqrt(globalVectorRProduct / globalVectorBProduct) > ctx.epsilon)
    {
        for (int i = 0; i < ctx.pcount; i++)
        {
            if (ctx.rank != i)
            {
                MPI_Isend(localVectorZ.data(), ctx.rowCounts[ctx.rank], MPI_DOUBLE,
                          i, 0, MPI_COMM_WORLD, &requests[i]);
            }
        }

        std::fill(localVectorAZ.begin(), localVectorAZ.end(), 0.0);

        for (int i = 0; i < ctx.pcount; i++)
        {
            if (ctx.rank != i)
            {
                otherVectorZ.resize(ctx.rowCounts[i]);
                MPI_Recv(otherVectorZ.data(), ctx.rowCounts[i], MPI_DOUBLE,
                         i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                multiply(localMatrixA, otherVectorZ, localVectorTemp, ctx.rowIndexes[i]);
                add(localVectorAZ, localVectorTemp, localVectorAZ);
            }
            else
            {
                multiply(localMatrixA, localVectorZ, localVectorTemp, ctx.rowIndexes[i]);
                add(localVectorAZ, localVectorTemp, localVectorAZ);
            }
        }

        double localVectorAZZProduct = dotVecVec(localVectorAZ, localVectorZ);
        double globalVectorAZZProduct = 0;
        MPI_Allreduce(&localVectorAZZProduct, &globalVectorAZZProduct, 1,
                      MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

        double scalarA = globalVectorRProduct / globalVectorAZZProduct;

        multiply(scalarA, localVectorZ, localVectorTemp);
        add(localVectorX, localVectorTemp, localVectorX);

        multiply(scalarA, localVectorAZ, localVectorTemp);
        substract(localVectorR, localVectorTemp, localVectorRNext);

        double localVectorRProductNext = dotVecVec(localVectorRNext, localVectorRNext);
        double globalVectorRProductNext = 0;
        MPI_Allreduce(&localVectorRProductNext, &globalVectorRProductNext, 1,
                      MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

        double scalarB = globalVectorRProductNext / globalVectorRProduct;

        multiply(scalarB, localVectorZ, localVectorTemp);
        add(localVectorRNext, localVectorTemp, localVectorZ);

        std::swap(localVectorR, localVectorRNext);
        globalVectorRProduct = globalVectorRProductNext;
    }

    auto duration = MPI_Wtime() - timeStart;

    MathStructs::Vector globalVectorX(ctx.n);
    MPI_Allgatherv(localVectorX.data(), ctx.rowCounts[ctx.rank], MPI_DOUBLE,
                   globalVectorX.data(), ctx.rowCounts.data(), ctx.rowIndexes.data(),
                   MPI_DOUBLE, MPI_COMM_WORLD);

    if (ctx.rank == 0)
    {
        MathStructs::Matrix matrixA = generateLocalMatrix(ctx.n, 0, ctx.n);
        MathStructs::Vector globalVectorB = generateLocalVector(ctx.n, 0, ctx.n);
        std::cout << "Time spent: " << duration << " seconds\n";
        std::cout << "inaccuracy: "
            << checkAnswer(matrixA, globalVectorB, globalVectorX) << '\n';
    }
}
