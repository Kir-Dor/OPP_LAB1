#pragma once

#include "CheckAnswer.hpp"
#include "Generate.hpp"
#include "MathOp.hpp"
#include "Structs.hpp"
#include <mpi.h>
#include <vector>

struct TaskContext {
  std::vector<int> rowCounts, rowIndexes;
  double epsilon;
  int n, rank, pcount;
};

inline void solveParVariant1(const TaskContext &ctx) {
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

  while (lenVec(vectorR) / lenVec(vectorB) > ctx.epsilon) {
    multiply(localMatrixA, vectorZ, localVectorAZ);
    MPI_Allgatherv(localVectorAZ.data(), localVectorAZ.size(), MPI_DOUBLE,
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

  if (ctx.rank == 0) {
    MathStructs::Matrix matrixA = generateLocalMatrix(ctx.n, 0, ctx.n);
    std::cout << "Time spent: " << duration << " seconds\n";
    std::cout << "inaccuracy: " << checkAnswer(matrixA, vectorB, vectorX)
              << '\n';
  }
}

inline void solveParVariant2(const TaskContext &ctx) {
  MathStructs::Matrix localMatrixA = generateLocalMatrix(
      ctx.n, ctx.rowIndexes[ctx.rank], ctx.rowCounts[ctx.rank]);

  MathStructs::Vector globalVectorB(ctx.n);
  MathStructs::Vector localVectorX(ctx.rowCounts[ctx.rank]);
  MathStructs::Vector localVectorB = generateLocalVector(
      ctx.n, ctx.rowIndexes[ctx.rank], ctx.rowCounts[ctx.rank]);

  auto timeStart = MPI_Wtime();

  double localVectorBProduct = dotVecVec(localVectorB, localVectorB);
  double globalVectorBProduct;
  MPI_Allreduce(&localVectorBProduct, &globalVectorBProduct, 1, MPI_DOUBLE,
                MPI_SUM, MPI_COMM_WORLD);

  double globalVectorRProduct = globalVectorBProduct;

  MathStructs::Vector localVectorR = localVectorB;
  MathStructs::Vector localVectorZ = localVectorR;
  MathStructs::Vector localVectorAZ(ctx.rowCounts[ctx.rank]);
  MathStructs::Vector localVectorTemp(ctx.rowCounts[ctx.rank]);
  MathStructs::Vector localVectorRNext(ctx.rowCounts[ctx.rank]);

  MathStructs::Vector globalVectorZ(ctx.n);
  MathStructs::Vector globalVectorAZ(ctx.n);

  while (std::sqrt(globalVectorRProduct) / std::sqrt(globalVectorBProduct) >
         ctx.epsilon) {
    MPI_Allgatherv(localVectorZ.data(), localVectorZ.size(), MPI_DOUBLE,
                   globalVectorZ.data(), ctx.rowCounts.data(),
                   ctx.rowIndexes.data(), MPI_DOUBLE, MPI_COMM_WORLD);

    multiply(localMatrixA, globalVectorZ, localVectorAZ);

    MPI_Allgatherv(localVectorAZ.data(), localVectorAZ.size(), MPI_DOUBLE,
                   globalVectorAZ.data(), ctx.rowCounts.data(),
                   ctx.rowIndexes.data(), MPI_DOUBLE, MPI_COMM_WORLD);

    double scalarA =
        globalVectorRProduct / dotVecVec(globalVectorAZ, globalVectorZ);

    multiply(scalarA, localVectorZ, localVectorTemp);
    add(localVectorX, localVectorTemp, localVectorX);

    multiply(scalarA, localVectorAZ, localVectorTemp);
    substract(localVectorR, localVectorTemp, localVectorRNext);

    double localVectorRNextProduct =
        dotVecVec(localVectorRNext, localVectorRNext);
    double globalVectorRNextProduct;
    MPI_Allreduce(&localVectorRNextProduct, &globalVectorRNextProduct, 1,
                  MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    double scalarB = globalVectorRNextProduct / globalVectorRProduct;

    multiply(scalarB, localVectorZ, localVectorTemp);
    add(localVectorRNext, localVectorTemp, localVectorZ);

    std::swap(localVectorR, localVectorRNext);
    globalVectorRProduct = globalVectorRNextProduct;
  }

  auto duration = MPI_Wtime() - timeStart;

  MathStructs::Vector globalVectorX(ctx.n);
  MPI_Gatherv(localVectorX.data(), localVectorX.size(), MPI_DOUBLE,
              globalVectorX.data(), ctx.rowCounts.data(), ctx.rowIndexes.data(),
              MPI_DOUBLE, 0, MPI_COMM_WORLD);

  MPI_Gatherv(localVectorB.data(), localVectorB.size(), MPI_DOUBLE,
              globalVectorB.data(), ctx.rowCounts.data(), ctx.rowIndexes.data(),
              MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if (ctx.rank == 0) {
    MathStructs::Matrix matrixA = generateLocalMatrix(ctx.n, 0, ctx.n);
    std::cout << "Time spent: " << duration << " seconds\n";
    std::cout << "inaccuracy: "
              << checkAnswer(matrixA, globalVectorB, globalVectorX) << '\n';
  }
}
