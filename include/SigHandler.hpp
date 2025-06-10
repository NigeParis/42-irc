#pragma once
#include "signal.h"

class SigHandler {
    public:
        SigHandler(void);
        ~SigHandler(void);
        static bool sigloop;
        static void handleStop(int signal);
};