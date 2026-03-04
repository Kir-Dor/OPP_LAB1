#pragma once

#include "Structs.hpp"

#include <algorithm>
#include <cmath>

template <typename InIt1, typename InIt2, typename ReduceOp,
          typename TransformOp>
inline double transformReduce(InIt1 first1, InIt1 last1, InIt2 first2,
                              double init, ReduceOp reduceOp,
                              TransformOp transformOp) {
  while (first1 != last1) {
    init = reduceOp(init, transformOp(*first1, *first2));

    ++first1;
    ++first2;
  }

  return init;
}

inline void multiply(const MathStructs::Matrix &mat,
                     const MathStructs::Vector &vec,
                     MathStructs::Vector &result) {
  auto row_count = mat.row_count();
  auto column_count = mat.column_count();

  for (size_t i = 0; i < mat.row_count(); ++i) {
    result[i] = transformReduce(
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
  return transformReduce(vec1.begin(), vec1.end(), vec2.begin(), 0.0,
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
