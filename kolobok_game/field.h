#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <iostream>

struct TextureManager {
    std::unordered_map<std::string, sf::Texture> textures;

    bool loadTexture(const std::string& id, const std::string& filePath) {
        sf::Texture texture;
        if (!texture.loadFromFile(filePath)) {
            std::cerr << "Не удалось загрузить текстуру: " << filePath << std::endl;
            return false;
        }

        textures[id] = texture;
        return true;
    }

    sf::Texture& getTexture(const std::string& id) {
        return textures.at(id);
    }
};

enum struct CellCategory {
    WALL,
    COOKIE,
    EMPTY,
    SUPER_COOKIE,
};

struct Cell {
    CellCategory category;
    sf::RectangleShape bounds;
};

struct Field {
    size_t width = 0;
    size_t height = 0;
    Cell *cells = nullptr;
};

void fieldInit(Field& field, TextureManager &textureManager);
void drawField(sf::RenderWindow& window, Field& field, TextureManager &textureManager);
bool checkFieldWallsCollision(const Field &field, const sf::FloatRect &oldBounds, sf::Vector2f &offset);

sf::Vector2f findPacmanStartPosition(Field &field);
sf::Vector2f findGhostStartPosition(Field &field);