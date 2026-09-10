#include "Game.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

Game::Game(int widthValue, int heightValue, bool blackAndWhite)
    : width(widthValue),
      height(heightValue),
      blackAndWhiteMode(blackAndWhite),
      running(true),
      score(0),
      lives(3),
      frameCount(0),
      spawnInterval(25),
      rng(std::random_device{}()),
      xSpawnDistribution(1, widthValue - 2),
      enemyTypeDistribution(0, 5) {
    resetState();
}

void Game::resetState() {
    player = {width / 2, height - 2};
    bullets.clear();
    enemies.clear();
    score = 0;
    lives = 3;
    frameCount = 0;
    spawnInterval = 25;
}

void Game::run() {
    terminal.setup();
    running = true;

    while (running) {
        processInput();
        update();
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    terminal.restore();
    std::cout << color("red") << "\nGame Over! Final Score: " << score << color("reset") << "\n";
}

void Game::processInput() {
    int input = terminal.readInput();

    if (input == -1) {
        return;
    }

    if (input == 'q' || input == 'Q') {
        running = false;
        return;
    }

    if (input == 'L' || input == 'a' || input == 'A') {
        if (player.x > 1) {
            player.x -= 1;
            playMoveSound();
        }
    } else if (input == 'R' || input == 'd' || input == 'D') {
        if (player.x < width - 2) {
            player.x += 1;
            playMoveSound();
        }
    } else if (input == ' ') {
        bullets.push_back({player.x, player.y - 1});
        playShootSound();
    }
}

void Game::update() {
    frameCount += 1;

    if (frameCount % spawnInterval == 0) {
        spawnEnemy();
    }

    moveBullets();
    moveEnemies();
    detectCollisions();
    increaseDifficulty();

    if (lives <= 0) {
        running = false;
    }
}

void Game::moveBullets() {
    for (std::size_t i = 0; i < bullets.size(); ++i) {
        bullets[i].y -= 1;
    }

    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(), [](const Entity& bullet) { return bullet.y < 1; }),
        bullets.end());
}

void Game::moveEnemies() {
    int speedStep = std::max(3, 8 - (score / 8));

    for (std::size_t i = 0; i < enemies.size(); ++i) {
        enemies[i].speedCounter += 1;
        if (enemies[i].speedCounter >= speedStep) {
            enemies[i].body.y += 1;
            enemies[i].speedCounter = 0;
        }

        if (enemies[i].body.y == player.y && enemies[i].body.x == player.x) {
            lives -= 1;
            enemies[i].body.y = height;
            playCollisionSound();
        }
    }

    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(), [this](const Enemy& enemy) { return enemy.body.y >= height - 1; }),
        enemies.end());
}

void Game::spawnEnemy() {
    Enemy enemy;
    enemy.body = {xSpawnDistribution(rng), 1};
    enemy.speedCounter = 0;
    enemy.type = enemyTypeDistribution(rng);
    enemies.push_back(enemy);
}

void Game::detectCollisions() {
    std::vector<Entity> keptBullets;

    for (std::size_t bulletIndex = 0; bulletIndex < bullets.size(); ++bulletIndex) {
        bool collided = false;
        for (std::size_t enemyIndex = 0; enemyIndex < enemies.size(); ++enemyIndex) {
            if (bullets[bulletIndex].x == enemies[enemyIndex].body.x && bullets[bulletIndex].y == enemies[enemyIndex].body.y) {
                score += 10;
                enemies.erase(enemies.begin() + static_cast<long>(enemyIndex));
                collided = true;
                playCollisionSound();
                break;
            }
        }

        if (!collided) {
            keptBullets.push_back(bullets[bulletIndex]);
        }
    }

    bullets = keptBullets;
}

void Game::increaseDifficulty() {
    int adjusted = 25 - (score / 30) - (frameCount / 500);
    spawnInterval = std::max(8, adjusted);
}

void Game::render() {
    clearScreen();

    std::vector<std::string> board(static_cast<std::size_t>(height), std::string(static_cast<std::size_t>(width), ' '));

    for (int y = 0; y < height; ++y) {
        board[static_cast<std::size_t>(y)][0] = '|';
        board[static_cast<std::size_t>(y)][static_cast<std::size_t>(width - 1)] = '|';
    }
    for (int x = 0; x < width; ++x) {
        board[0][static_cast<std::size_t>(x)] = '-';
        board[static_cast<std::size_t>(height - 1)][static_cast<std::size_t>(x)] = '-';
    }

    board[static_cast<std::size_t>(player.y)][static_cast<std::size_t>(player.x)] = 'A';

    for (std::size_t i = 0; i < bullets.size(); ++i) {
        if (bullets[i].y > 0 && bullets[i].y < height - 1) {
            board[static_cast<std::size_t>(bullets[i].y)][static_cast<std::size_t>(bullets[i].x)] = '|';
        }
    }

    for (std::size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i].body.y > 0 && enemies[i].body.y < height - 1) {
            board[static_cast<std::size_t>(enemies[i].body.y)][static_cast<std::size_t>(enemies[i].body.x)] = 'O';
        }
    }

    std::cout << color("white") << "Score: " << score << "  Lives: " << lives
              << "  Controls: A/D or arrows, SPACE shoot, Q quit" << color("reset") << "\n";

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            char c = board[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
            if (c == 'A') {
                std::cout << color("cyan") << c << color("reset");
            } else if (c == '|') {
                std::cout << color("green") << c << color("reset");
            } else if (c == 'O') {
                int colorIndex = 0;
                for (std::size_t i = 0; i < enemies.size(); ++i) {
                    if (enemies[i].body.y == y && enemies[i].body.x == x) {
                        colorIndex = enemies[i].type;
                        break;
                    }
                }
                if (colorIndex == 0) {
                    std::cout << color("red") << c << color("reset");
                } else if (colorIndex == 1) {
                    std::cout << color("yellow") << c << color("reset");
                } else if (colorIndex == 2) {
                    std::cout << color("blue") << c << color("reset");
                } else if (colorIndex == 3) {
                    std::cout << color("white") << c << color("reset");
                } else if (colorIndex == 4) {
                    std::cout << color("green") << c << color("reset");
                } else {
                    std::cout << color("cyan") << c << color("reset");
                }
            } else {
                std::cout << c;
            }
        }
        std::cout << "\n";
    }

    std::cout.flush();
}

void Game::playMoveSound() {
    std::cout << '\a';
}

void Game::playShootSound() {
    std::cout << '\a';
}

void Game::playCollisionSound() {
    std::cout << '\a' << '\a';
}

std::string Game::color(const std::string& name) const {
    if (blackAndWhiteMode) {
        return "";
    }

    if (name == "red") {
        return "\033[31m";
    }
    if (name == "cyan") {
        return "\033[36m";
    }
    if (name == "yellow") {
        return "\033[33m";
    }
    if (name == "green") {
        return "\033[32m";
    }
    if (name == "blue") {
        return "\033[34m";
    }
    if (name == "white") {
        return "\033[37m";
    }
    return "\033[0m";
}

void Game::clearScreen() const {
    std::cout << "\033[2J\033[H";
}
