#pragma once

#include "MathOp.hpp"

inline double checkAnswer(const MathStructs::Matrix &leMatrix,
                          const MathStructs::Vector &leVector,
                          const MathStructs::Vector &answer) {
  auto N = answer.size();

  MathStructs::Vector calcLEVector(N);
  multiply(leMatrix, answer, calcLEVector);

  auto accuracy = transformReduce(
      leVector.begin(), leVector.end(), calcLEVector.begin(), 0.0, std::plus(),
      [](auto x, auto y) { return std::abs(x - y); });

  auto accuracy_rate = accuracy / N;

  return accuracy_rate;
}
