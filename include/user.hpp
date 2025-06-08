/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nige42 <nige42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 15:01:27 by nige42            #+#    #+#             */
/*   Updated: 2025/06/08 16:56:04 by nige42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>

class User {

    public:

        User (int fd, short events, short revents);
        ~User(void);

        void setUserFd(int fd);
        int getUserFd(void);

        
    private:

        int socketFd_;
        
    
    
};