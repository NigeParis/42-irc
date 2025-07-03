/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 14:30:43 by nrobinso          #+#    #+#             */
/*   Updated: 2025/07/03 14:30:46 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client(int fd, int guest_number)
    : socket_fd(fd), nickname("Guest" + intToString(guest_number)),
      username("default"), hostname("default"), is_authenticated(false){
        quote = Quote();
      }

Client::~Client() { close(socket_fd); }
