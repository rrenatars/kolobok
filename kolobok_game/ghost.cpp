#pragma once

#include "ghost.h"
#include <iostream>
#include <cstdlib>

constexpr float GHOST_SPEED = 100.f;

const sf::Color GHOST_COLOR = sf::Color(0x23, 0xCC, 0x00);

const float GHOST_SIZE = 45.f;

void setGhostTextureById(Ghost& ghost, TextureManager& textureManager, size_t i) {
    std::string textureId;
    switch (i) {
        case 0:
            textureId = "fox";
            break;
        case 1:
            textureId = "rabbit";
            break;
        case 2:
            textureId = "wolf";
            break;
        case 3:
            textureId = "bear";
            break;
    }

    ghost.shape.setTexture(&textureManager.getTexture(textureId));
}

void ghostInit(Ghost &ghost, sf::Vector2f startPosition, size_t i, TextureManager &textureManager) {
    textureManager.loadTexture("fox", "res/fox.png");
    textureManager.loadTexture("rabbit", "res/rabbit.png");
    textureManager.loadTexture("bear", "res/bear.png");
    textureManager.loadTexture("wolf", "res/wolf.png");

    ghost.direction = Direction::UP;
    ghost.shape.setSize({GHOST_SIZE, GHOST_SIZE});
    setGhostTextureById(ghost, textureManager, i);
    ghost.shape.setPosition(startPosition);
}

void updateGhostPosition(Ghost& ghost, float elapsedTime, Field &field) {
    const float offset = GHOST_SPEED * elapsedTime;
    sf::Vector2f movement(0.f, 0.f);

    switch(ghost.direction) {
        case Direction::UP: 
            movement.y -= offset;
            break;
        case Direction::DOWN:
            movement.y += offset;
            break;
        case Direction::RIGHT:
            movement.x += offset;
            break;  
        case Direction::LEFT:
            movement.x -= offset;
            break;
        case Direction::NONE:
            break;
    }

    const sf::FloatRect ghostBounds = ghost.shape.getGlobalBounds();
    if (checkFieldWallsCollision(field, ghostBounds, movement))
    {
        int randomDirection = rand() % 4;

        switch (randomDirection) {
            case 0:
                ghost.direction = Direction::UP;
                break;
            case 1:
                ghost.direction = Direction::DOWN;
                break;
            case 2:
                ghost.direction = Direction::RIGHT;
                break;
            case 3:
                ghost.direction = Direction::LEFT;
                break;
        }
    }
     
    ghost.shape.move(movement);
}
