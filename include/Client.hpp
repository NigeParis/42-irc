/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 14:34:29 by nrobinso          #+#    #+#             */
/*   Updated: 2025/07/03 14:34:32 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Utils.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include "Quote.hpp"

class Quote;

class Client {

public:
  int socket_fd;
  std::string nickname;
  std::string username;
  std::string hostname;
  bool is_authenticated;
  std::string buffer;
  Quote quote;

  Client(int fd, int guest_number);
  ~Client();
};
