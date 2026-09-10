#ifndef GAME_HPP
#define GAME_HPP

#include <random>
#include <string>
#include <vector>

#include "Terminal.hpp"

struct Entity {
    int x;
    int y;
};

struct Enemy {
    Entity body;
    int speedCounter;
    int type;
};

class Game {
public:
    Game(int width, int height, bool blackAndWhiteMode);
    void run();

private:
    int width;
    int height;
    bool blackAndWhiteMode;
    bool running;

    Entity player;
    std::vector<Entity> bullets;
    std::vector<Enemy> enemies;

    int score;
    int lives;
    int frameCount;
    int spawnInterval;

    Terminal terminal;
    std::mt19937 rng;
    std::uniform_int_distribution<int> xSpawnDistribution;
    std::uniform_int_distribution<int> enemyTypeDistribution;

    void resetState();
    void processInput();
    void update();
    void render();

    void moveEnemies();
    void moveBullets();
    void spawnEnemy();
    void detectCollisions();
    void increaseDifficulty();

    void playMoveSound();
    void playShootSound();
    void playCollisionSound();

    std::string color(const std::string& name) const;
    void clearScreen() const;
};

#endif
