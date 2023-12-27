#pragma once

#include <SFML/Graphics.hpp>

struct Field;
struct TextureManager;

enum struct Direction {
    NONE,
    UP,
    DOWN,
    RIGHT,
    LEFT
};

struct SuperCookie {
    bool isActiveSuper = false;
    sf::Clock superTimer;
};

struct Pacman {
    sf::CircleShape shape;
    Direction direction;
    int score;
    int hp;
    SuperCookie superCookie;
};

void pacmanInit(Pacman& pacman, sf::Vector2f startPosition, TextureManager& textureManager);
void updatePacmanPosition(Pacman& pacman, float elapsedTime, Field &field);
bool handleKeyPressed(const sf::Event::KeyEvent& event, Pacman& pacman);
bool eatCookie(Field& field, Pacman &pacman, const sf::FloatRect &bounds);