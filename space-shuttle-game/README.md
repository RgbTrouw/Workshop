# Space Shuttle Game

A complete console-based space shooter written in C++ using ANSI escape sequences for rendering and color.

**Made by Copilot**

## Features

- Real-time shuttle movement (left/right arrows or `A`/`D`) and shooting (`Space`)
- Continuous, non-blocking input loop
- Falling enemy astro-bodies/ships with increasing difficulty
- Score tracking and lives system
- System beep sound effects for movement, shooting, and collisions
- ANSI colored rendering (red, cyan, yellow, green, blue, white)
- Black-and-white mode (`--bw`)
- No static variables used in game code
- Cross-platform input implementation:
  - Linux/macOS: `termios` + non-blocking stdin
  - Windows: `conio` (`_kbhit`, `_getch`)

## Build

```bash
make
```

## Run

Color mode (default):

```bash
./space_shuttle_game
```

Black-and-white mode:

```bash
./space_shuttle_game --bw
```

## Controls

- Move left: `A` or Left Arrow
- Move right: `D` or Right Arrow
- Shoot: `Space`
- Quit: `Q`

## Project Structure

- `/include/Game.hpp` - game state and core gameplay class
- `/include/Terminal.hpp` - cross-platform terminal input handling
- `/src/Game.cpp` - game loop, rendering, collisions, difficulty
- `/src/Terminal.cpp` - non-blocking input setup and reads
- `/src/main.cpp` - entry point and CLI mode selection
- `/Makefile` - build instructions
- `/LICENSE` - MIT license
