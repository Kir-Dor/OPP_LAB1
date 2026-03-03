#pragma once

#include "Structs.hpp"
#include <cmath>

inline MathStructs::Matrix generateLocalMatrix(int n, int rowIndex,
                                               int rowCount) {
  MathStructs::Matrix result(rowCount, n);

  int x = n;
  for (int i = std::sqrt(n); i > 0; --i) {
    if (n % i == 0) {
      x = i;
      break;
    }
  }

  for (int i = rowIndex; i < rowIndex + rowCount; ++i) {
    result[(i - rowIndex) * n + i] = -4;
    if ((i * n + i) % x != 0) {
      result[(i - rowIndex) * n + i - 1] = 1;
    }
    if ((i * n + i) % x != x - 1) {
      result[(i - rowIndex) * n + i + 1] = 1;
    }
    if (i + x < n) {
      result[(i - rowIndex) * n + i + x] = 1;
    }
    if (i - x >= 0) {
      result[(i - rowIndex) * n + i - x] = 1;
    }
  }
  return result;
}

inline MathStructs::Vector generateLocalVector(int n, int pos, int count) {
  MathStructs::Vector result(count);

  for (int i = pos; i < pos + count; ++i) {
    if (i / (n / 10)) {
      result[i - pos] = 25;
    }
  }

  return result;
}
