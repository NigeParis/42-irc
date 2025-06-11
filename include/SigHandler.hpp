/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SigHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrobinso <nrobinso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:01:30 by nrobinso          #+#    #+#             */
/*   Updated: 2025/06/11 17:01:36 by nrobinso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "signal.h"

class SigHandler {
    public:
        SigHandler(void);
        ~SigHandler(void);
        static bool sigloop;
        static void handleStop(int signal);
};