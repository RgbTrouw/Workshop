#include "Terminal.hpp"

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

Terminal::Terminal() : configured(false)
#ifdef _WIN32
, originalMode(0)
#else
, originalFlags(0)
#endif
{
}

Terminal::~Terminal() {
    restore();
}

void Terminal::setup() {
    if (configured) {
        return;
    }
#ifdef _WIN32
    HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(inputHandle, &mode);
    originalMode = static_cast<int>(mode);
    mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    SetConsoleMode(inputHandle, mode);
#else
    tcgetattr(STDIN_FILENO, &originalTermios);
    termios raw = originalTermios;
    raw.c_lflag &= static_cast<unsigned int>(~(ICANON | ECHO));
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    originalFlags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, originalFlags | O_NONBLOCK);
#endif
    configured = true;
}

void Terminal::restore() {
    if (!configured) {
        return;
    }
#ifdef _WIN32
    HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleMode(inputHandle, static_cast<DWORD>(originalMode));
#else
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
    fcntl(STDIN_FILENO, F_SETFL, originalFlags);
#endif
    configured = false;
}

int Terminal::readInput() {
#ifdef _WIN32
    if (!_kbhit()) {
        return -1;
    }

    int key = _getch();
    if (key == 0 || key == 224) {
        int arrow = _getch();
        if (arrow == 75) {
            return 'L';
        }
        if (arrow == 77) {
            return 'R';
        }
    }
    return key;
#else
    char first = 0;
    const ssize_t amount = read(STDIN_FILENO, &first, 1);
    if (amount <= 0) {
        return -1;
    }

    if (first == '\x1b') {
        char second = 0;
        char third = 0;
        if (read(STDIN_FILENO, &second, 1) > 0 && read(STDIN_FILENO, &third, 1) > 0) {
            if (second == '[' && third == 'D') {
                return 'L';
            }
            if (second == '[' && third == 'C') {
                return 'R';
            }
        }
        return first;
    }

    return static_cast<int>(first);
#endif
}
