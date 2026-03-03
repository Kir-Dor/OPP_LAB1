#pragma once

#include "Structs.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>

inline void multiply(const MathStructs::Matrix &mat,
                     const MathStructs::Vector &vec,
                     MathStructs::Vector &result) {
  auto row_count = mat.row_count();
  auto column_count = mat.column_count();

  for (size_t i = 0; i < mat.row_count(); ++i) {
    result[i] = std::transform_reduce(
        mat.begin() + i * column_count, mat.begin() + (i + 1) * column_count,
        vec.begin(), 0.0, std::plus(), std::multiplies());
  }
}

inline void add(const MathStructs::Vector &vec1,
                const MathStructs::Vector &vec2, MathStructs::Vector &result) {
  std::transform(vec1.begin(), vec1.end(), vec2.begin(), result.begin(),
                 std::plus());
}

inline void substract(const MathStructs::Vector &vec1,
                      const MathStructs::Vector &vec2,
                      MathStructs::Vector &result) {
  std::transform(vec1.begin(), vec1.end(), vec2.begin(), result.begin(),
                 std::minus());
}

inline double dotVecVec(const MathStructs::Vector &vec1,
                        const MathStructs::Vector &vec2) {
  return std::transform_reduce(vec1.begin(), vec1.end(), vec2.begin(), 0.0,
                               std::plus(), std::multiplies());
}

inline void multiply(double scal, const MathStructs::Vector &vec,
                     MathStructs::Vector &result) {
  std::transform(vec.begin(), vec.end(), result.begin(),
                 [scal](double x) { return x * scal; });
}

inline double lenVec(const MathStructs::Vector &vec) {
  return std::sqrt(dotVecVec(vec, vec));
}
