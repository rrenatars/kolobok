#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include "pacman.h"
#include "field.h"
#include "ghost.h"
#include <iostream>
#include <unordered_map>

constexpr unsigned WINDOW_WIDTH = 800;
constexpr unsigned WINDOW_HEIGHT = 900;

void renderText(sf::RenderWindow& window, const sf::Font& font, const std::string& text, const sf::Vector2f& position, const sf::Color& color) {
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(color);
    scoreText.setString(text);
    scoreText.setPosition(position);

    window.draw(scoreText);
}

void applySuperCookieEffect(Ghost (&ghosts)[4], Pacman& pacman, TextureManager &textureManager) {
    if (pacman.superCookie.isActiveSuper) {
        for (size_t i = 0; i < 4; ++i) {
            ghosts[i].shape.setFillColor({20, 20, 20});
        }

        sf::Time elapsedTime = pacman.superCookie.superTimer.getElapsedTime();
        if (elapsedTime.asSeconds() >= 7.0) {
            pacman.superCookie.isActiveSuper = false;
            pacman.superCookie.superTimer.restart();

            for (size_t i = 0; i < 4; ++i) {
                ghosts[i].shape.setFillColor({255, 255, 255});
            }
        }
    }
}

void handleCollision(Pacman& pacman, Ghost& ghost, Field& field) {
    const sf::FloatRect pacmanBounds = pacman.shape.getGlobalBounds();
    const sf::FloatRect ghostBounds = ghost.shape.getGlobalBounds();

    if (pacmanBounds.intersects(ghostBounds)) {
        if (pacman.superCookie.isActiveSuper) {
            pacman.score += 50;
            ghost.shape.setPosition(findGhostStartPosition(field));
        } else {
            pacman.hp--;
            sf::Vector2f initialPosition = findPacmanStartPosition(field);
            pacman.shape.setPosition(initialPosition);
            pacman.direction = Direction::NONE;
        }
    }
}

void handleUpdate(sf::Clock &clock, Pacman& pacman, Field &field, Ghost (&ghosts)[4], TextureManager &textureManager) {
    sf::Vector2f speed = {10, 0};

    const float elapsedSeconds = clock.getElapsedTime().asSeconds();
    clock.restart();
    updatePacmanPosition(pacman, elapsedSeconds, field);

    applySuperCookieEffect(ghosts, pacman, textureManager);

    for (size_t i = 0; i < 4; ++i) {
        updateGhostPosition(ghosts[i], elapsedSeconds, field);
        handleCollision(pacman, ghosts[i], field);
    }
}


void gameRender(sf::RenderWindow& window, Pacman& pacman, Field& field, Ghost (&ghosts)[4], TextureManager &textureManager, const sf::Font& font) {
    window.clear();
    drawField(window, field, textureManager);
    window.draw(pacman.shape);
    for (size_t i = 0; i < 4; ++i) {
        window.draw(ghosts[i].shape);
    }

    renderText(window, font, "Score: " + std::to_string(pacman.score), {WINDOW_WIDTH - 300, WINDOW_HEIGHT - 100}, sf::Color::White);

    for (int i = 0; i < pacman.hp; ++i) {
        sf::CircleShape pacmanSprite;
        pacmanSprite.setTexture(&textureManager.getTexture("pacman")); 
        pacmanSprite.setRadius(20.f);
        pacmanSprite.setPosition({50.f * i, WINDOW_HEIGHT - 100});
        window.draw(pacmanSprite);
    }

    window.display();
}

void handleEvents(sf::RenderWindow &window, Pacman& pacman) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape || pacman.hp < 1) {
            window.close();
        } else if (event.type == sf::Event::KeyPressed) {
            handleKeyPressed(event.key, pacman);
        }
    }
}

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Kolobok", sf::Style::Close, settings);
    window.setFramerateLimit(60);

    sf::Clock clock;

    TextureManager textureManager;
    Field field;
    fieldInit(field, textureManager);

    sf::Font font;
    if (!font.loadFromFile("res/ArcadeR.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return EXIT_FAILURE;
    }


    sf::Vector2f pacmanStartPosition = findPacmanStartPosition(field);

    Pacman pacman;
    pacmanInit(pacman, pacmanStartPosition, textureManager);

    sf::Vector2f ghostStartPosition = findGhostStartPosition(field);
    Ghost ghosts[NUM_GHOSTS];
    for (size_t i = 0; i < NUM_GHOSTS; ++i) {
        ghostInit(ghosts[i], {ghostStartPosition.x + i * 40.f, ghostStartPosition.y}, i, textureManager);
    }

    while (window.isOpen()) {
        handleEvents(window, pacman);

        handleUpdate(clock, pacman, field, ghosts, textureManager);

        gameRender(window, pacman, field, ghosts, textureManager, font);
    }
}