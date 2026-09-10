#include <iostream>
#include <string>

#include "Game.hpp"

int main(int argc, char** argv) {
    bool blackAndWhite = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--bw" || arg == "--black-and-white") {
            blackAndWhite = true;
        }
    }

    std::cout << "Space Shuttle Shooter\n";
    std::cout << "Use A/D or arrow keys to move, SPACE to shoot, Q to quit.\n";
    std::cout << "Mode: " << (blackAndWhite ? "Black and White" : "Color") << "\n";

    Game game(50, 24, blackAndWhite);
    game.run();

    return 0;
}
