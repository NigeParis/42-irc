/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quote.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 14:34:42 by nrobinso          #+#    #+#             */
/*   Updated: 2025/07/03 14:34:44 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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