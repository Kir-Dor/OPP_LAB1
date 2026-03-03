#pragma once

#include <iostream>
#include <vector>

namespace MathStructs {
class Matrix : public std::vector<double> {
public:
  Matrix(size_t row_count, size_t column_count_)
      : std::vector<double>(row_count * column_count_), row_count_(row_count),
        column_count_(column_count_) {}

  void print() const {
    auto it = this->begin();
    for (auto y = 0; y < row_count_; ++y) {
      for (auto x = 0; x < column_count_; ++x) {
        std::cout << *it << " ";
        ++it;
      }
      std::cout << std::endl;
    }
  }

  size_t row_count() const { return this->row_count_; }
  size_t column_count() const { return this->column_count_; }

private:
  size_t row_count_;
  size_t column_count_;
};

class Vector : public std::vector<double> {
  using std::vector<double>::vector;

public:
  void print() const {
    for (auto it = this->begin(); it != this->end() - 1; ++it) {
      std::cout << *it << " ";
    }
    std::cout << *(this->end() - 1) << std::endl;
  }
};
} // namespace MathStructs
