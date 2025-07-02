#pragma once

#include <string>
#include <vector>
#include "Utils.hpp"

class Quote {
  public:
    Quote();
		~Quote();
    std::string getRandomQuote() const;
		void setQuote(const std::string &message);
		void removeQuote(const std::string &index);
		std::vector<std::string> getQuotes() const;
  private:
    std::vector<std::string> quotes;
};