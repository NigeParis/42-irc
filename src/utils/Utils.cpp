#include "../../include/Utils.hpp"

std::string intToString(int number) {
  std::ostringstream oss;
  oss << number;
  return oss.str();
}

std::vector<std::string> split(const std::string &str,
                               const std::string &delim) {
  std::vector<std::string> tokens;
  size_t start = 0;
  size_t end = 0;

  while ((end = str.find(delim, start)) != std::string::npos) {
    std::string token = str.substr(start, end - start);
    if (!token.empty()) {
      tokens.push_back(token);
    }
    start = end + delim.length();
  }

  std::string lastToken = str.substr(start);
  if (!lastToken.empty()) {
    tokens.push_back(lastToken);
  }

  return tokens;
}
