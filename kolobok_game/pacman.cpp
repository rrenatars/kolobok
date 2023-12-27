#include "pacman.h"
#include "field.h"
#include <iostream>

constexpr float PACMAN_SPEED = 100.f;

const sf::Color PACMAN_COLOR = sf::Color(0xFF, 0xFF, 0x00);

const float PACMAN_SIZE = 20.f;

void pacmanInit(Pacman& pacman, sf::Vector2f startPosition, TextureManager& textureManager) {
    pacman.direction = Direction::NONE;
    pacman.shape.setRadius(PACMAN_SIZE);

    pacman.score = 0;
    pacman.hp = 3;
    pacman.superCookie.isActiveSuper = false;

    if (textureManager.loadTexture("pacman", "res/kolobok.jpg")) {
        pacman.shape.setTexture(&textureManager.getTexture("pacman"));
    }

    pacman.shape.setPosition(startPosition);
}

void updatePacmanPosition(Pacman& pacman, float elapsedTime, Field &field) {
    const float offset = PACMAN_SPEED * elapsedTime;
    sf::Vector2f movement(0.f, 0.f);

    switch(pacman.direction) {
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

    const sf::FloatRect pacmanBounds = pacman.shape.getGlobalBounds();
    if (checkFieldWallsCollision(field, pacmanBounds, movement))
    {
        pacman.direction = Direction::NONE;
    }

    if (eatCookie(field, pacman, pacmanBounds)) {
        pacman.score += 5;
    }
    
    pacman.shape.move(movement);
}

bool handleKeyPressed(const sf::Event::KeyEvent& event, Pacman& pacman) {
    bool handled = true;

    switch (event.code) {
        case sf::Keyboard::W:
        case sf::Keyboard::Up:
            pacman.direction = Direction::UP;
            break;
        case sf::Keyboard::S:
        case sf::Keyboard::Down:
            pacman.direction = Direction::DOWN;
            break;
        case sf::Keyboard::D:
        case sf::Keyboard::Right:
            pacman.direction = Direction::RIGHT;
            break;
        case sf::Keyboard::A:
        case sf::Keyboard::Left:
            pacman.direction = Direction::LEFT;
            break;
        default:
            handled = false;
            break;
    }

    return handled;
}

bool eatCookie(Field& field, Pacman &pacman, const sf::FloatRect &bounds) {
    static const float COOKIE_AREA = 1.f;

    for (size_t i = 0, n = field.width * field.height; i < n; i++) {
        if (field.cells[i].category == CellCategory::EMPTY || field.cells[i].category == CellCategory::WALL) {
            continue;
        } else {
        
            sf::FloatRect cookieBounds = field.cells[i].bounds.getGlobalBounds();

            if (bounds.intersects(cookieBounds) && 
                bounds.left + bounds.width / 2 >= cookieBounds.left - COOKIE_AREA &&
                bounds.top + bounds.height / 2 >= cookieBounds.top - COOKIE_AREA &&
                bounds.left + bounds.width / 2 <= cookieBounds.left + cookieBounds.width + COOKIE_AREA &&
                bounds.top + bounds.height / 2 <= cookieBounds.top + cookieBounds.height + COOKIE_AREA) {
                if (field.cells[i].category == CellCategory::SUPER_COOKIE) {
                    pacman.score += 100;
                    pacman.superCookie.isActiveSuper = true;
                    pacman.superCookie.superTimer.restart();
                }
                field.cells[i].category = CellCategory::EMPTY;
                return true;
            }
        }
    }

    return false;
}