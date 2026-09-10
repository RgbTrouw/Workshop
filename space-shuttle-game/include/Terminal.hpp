#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#ifndef _WIN32
#include <termios.h>
#endif

class Terminal {
public:
    Terminal();
    ~Terminal();

    void setup();
    void restore();
    int readInput();

private:
    bool configured;
#ifdef _WIN32
    int originalMode;
#else
    int originalFlags;
    termios originalTermios;
#endif
};

#endif
