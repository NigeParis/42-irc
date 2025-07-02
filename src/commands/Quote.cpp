#include "../../include/Quote.hpp"
#include <cstdlib>
#include <ctime>

Quote::Quote() {
  // Initialize random seed
  srand(time(NULL));
  
  // 10 English jokes/quotes
  quotes.push_back("Why don't scientists trust atoms? Because they make up everything!");
  quotes.push_back("I told my wife she was drawing her eyebrows too high. She looked surprised.");
  quotes.push_back("Why don't programmers like nature? It has too many bugs.");
  quotes.push_back("I'm reading a book about anti-gravity. It's impossible to put down!");
  quotes.push_back("Why did the scarecrow win an award? He was outstanding in his field!");
  quotes.push_back("I used to hate facial hair, but then it grew on me.");
  quotes.push_back("Why don't eggs tell jokes? They'd crack each other up!");
  quotes.push_back("What do you call a fake noodle? An impasta!");
  quotes.push_back("I'm terrified of elevators, so I'm going to start taking steps to avoid them.");
  quotes.push_back("Why did the coffee file a police report? It got mugged!");
}

Quote::~Quote() {
  quotes.clear();
}

std::string Quote::getRandomQuote() const {
	
  if (quotes.empty()) {
    return "No quotes available!";
  }
  
  int randomIndex = rand() % quotes.size();
  return quotes[randomIndex];
}

void Quote::setQuote(const std::string &message) {
  if (!message.empty() && message.length() > 2) {
    quotes.push_back(message);
  }
}

void Quote::removeQuote(const std::string &index) {
  int idx = atoi(index.c_str());
  if (idx >= 0 && idx < static_cast<int>(quotes.size())) {
    quotes.erase(quotes.begin() + idx);
  }
}

std::vector<std::string> Quote::getQuotes() const {
  std::vector<std::string> quotes_str;
  for (size_t i = 0; i < quotes.size(); i++) {
    quotes_str.push_back(intToString(i) + "-" + quotes[i]);
  }
  return quotes_str;
}