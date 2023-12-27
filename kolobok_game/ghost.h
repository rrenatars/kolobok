#pragma once

#include <SFML/Graphics.hpp>
#include "field.h"
#include "pacman.h"

constexpr size_t NUM_GHOSTS = 4;

enum struct Direction;

struct Ghost {
    sf::RectangleShape shape;
    Direction direction;
};

void ghostInit(Ghost &ghost, sf::Vector2f startPosition, size_t i, TextureManager &textureManager);
void updateGhostPosition(Ghost& ghost, float elapsedTime, Field &field);
void setGhostTextureById(Ghost& ghost, TextureManager& textureManager, size_t i);