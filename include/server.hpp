/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nige42 <nige42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:54:08 by nige42            #+#    #+#             */
/*   Updated: 2025/06/05 10:27:06 by nige42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

class Server {

    public:
    
        Server(int port, std::string password);
        ~Server();
        void createServer(void);

    private:
    
        int port_;
        int socket_;
        std::string password_;

    
};
