#include "../include/Utils.hpp"

std::string intToString(int number) {
  std::ostringstream oss;
  oss << number;
  return oss.str();
}
