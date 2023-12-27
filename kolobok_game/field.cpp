#include "field.h"
#include <iostream>

static const char FIELD[] =      "#########################"
                                 "#           #           #"
                                 "# ### ##### # ##### ### #"
                                 "#           #           #"
                                 "# ### #  #######  # ### #"
                                 "#     #     #     #     #"
                                 "##### ###   #   ### #####"
                                 "!!!!# #           # #!!!!" 
                                 "!!!!# # #       # # #!!!!"
                                 "#####   #  GGGG #   #####"
                                 "#       #       #       #"
                                 "##### # ######### # #####"
                                 "!!!!# #           # #!!!!"
                                 "!!!!#               #!!!!"
                                 "##### ############# #####"
                                 "#                       #"
                                 "#     # ######### #     #"
                                 "#     #     #     #     #"
                                 "#  ####### ### #######  #"
                                 "#  #     #  @  #     #  #"
                                 "#  # ### ## # ## ### #  #"
                                 "#  # #      #      # #  #"
                                 "#  # # #### # #### # #  #"
                                 "#           #           #"
                                 "#########################";

static const float WALL_SIZE = 32.f;
static const float COOKIE_RADIUS = 5.f;
static const size_t FIELD_WIDTH = 25;
static const size_t FIELD_HEIGHT = 25;

static const sf::Color WALL_COLOR = sf::Color(0, 80, 0);
static const sf::Color ROAD_COLOR = sf::Color(40, 40, 40);
static const sf::Color COOKIE_COLOR = sf::Color(255, 255, 255);

static sf::FloatRect moveRect(const sf::FloatRect& rect, sf::Vector2f& offset)
{
    return { rect.left + offset.x, rect.top + offset.y, rect.width, rect.height };
}

sf::Vector2f findPacmanStartPosition(Field &field) {
    for (size_t y = 0; y < field.height; y++) {
        for (size_t x = 0; x < field.width; x++) {
            const size_t offset = y * field.width + x;
            if (FIELD[offset] == '@') {
                return {x * WALL_SIZE, y * WALL_SIZE};
            }
        }
    }
}

sf::Vector2f findGhostStartPosition(Field &field) {
    for (size_t y = 0; y < field.height; y++) {
        for (size_t x = 0; x < field.width; x++) {
            const size_t offset = y * field.width + x;
            if (FIELD[offset] == 'G') {
                return {x * WALL_SIZE, y * WALL_SIZE};
            }
        }
    }
}

bool spawnSuperCookie(float spawnChance) {
    float random = static_cast<float>(rand()) / RAND_MAX;
    return random < spawnChance;
}


void fieldInit(Field &field, TextureManager &textureManager) {
    field.width = FIELD_WIDTH;
    field.height = FIELD_HEIGHT;
    field.cells = new Cell[field.width * field.height];
    textureManager.loadTexture("ground", "res/ground.jpg");
    textureManager.loadTexture("evergreen", "res/evergreen-ground.png");
    for (size_t y = 0; y < field.height; y++) {
        for (size_t x = 0; x < field.width; x++) {
            const size_t offset = y * field.width + x;
            CellCategory category;
            sf::Color color;
            if (FIELD[offset] == '#') {
                category = CellCategory::WALL;
            } else if (FIELD[offset] == ' ') {
                category = spawnSuperCookie(0.01f) ? CellCategory::SUPER_COOKIE : CellCategory::COOKIE;
            }
            Cell& cell = field.cells[offset];
            cell.category = category;
            cell.bounds.setPosition(x * WALL_SIZE, y * WALL_SIZE);

            cell.bounds.setSize(sf::Vector2f(WALL_SIZE, WALL_SIZE));
            cell.bounds.setFillColor(WALL_COLOR);
        }
    }
}

void drawField(sf::RenderWindow &window, Field &field, TextureManager &textureManager) {
    for (size_t y = 0; y < field.height; y++) {
        for (size_t x = 0; x < field.width; x++) {
            const size_t offset = y * field.width + x;
            const Cell &cell = field.cells[offset];

            sf::RectangleShape shape;
            sf::CircleShape cookie;

            switch (cell.category) {
            case CellCategory::WALL:
                shape.setTexture(&textureManager.getTexture("evergreen"));
                shape.setSize({WALL_SIZE, WALL_SIZE});
                shape.setPosition(x * WALL_SIZE, y * WALL_SIZE);
                window.draw(shape);
                break;
            case CellCategory::COOKIE:
                cookie.setFillColor(COOKIE_COLOR);
                cookie.setRadius(COOKIE_RADIUS);
                cookie.setPosition(x * WALL_SIZE + 10, y * WALL_SIZE + 10);
                shape.setSize({WALL_SIZE, WALL_SIZE});
                shape.setTexture(&textureManager.getTexture("ground"));
                shape.setPosition(x * WALL_SIZE, y * WALL_SIZE);
                window.draw(shape);
                window.draw(cookie);
                break;
            case CellCategory::SUPER_COOKIE:
                cookie.setFillColor(COOKIE_COLOR);
                cookie.setRadius(10.f);
                cookie.setPosition(x * WALL_SIZE + 5, y * WALL_SIZE + 5);
                shape.setSize({WALL_SIZE, WALL_SIZE});
                shape.setTexture(&textureManager.getTexture("ground"));
                shape.setPosition(x * WALL_SIZE, y * WALL_SIZE);
                window.draw(shape);
                window.draw(cookie);
                break;
            case CellCategory::EMPTY:
                shape.setSize({WALL_SIZE, WALL_SIZE});
                shape.setTexture(&textureManager.getTexture("ground"));
                shape.setPosition(x * WALL_SIZE, y * WALL_SIZE);
                window.draw(shape);
            default:
                break;
            }
        }
    }
}

bool checkFieldWallsCollision(const Field& field, const sf::FloatRect& oldBounds, sf::Vector2f& offset)
{
    float tolerance = 8.f;

    sf::FloatRect newBounds = moveRect(oldBounds, offset);
    bool changed = false;

    for (size_t i = 0, n = field.width * field.height; i < n; i++)
    {
        const Cell& cell = field.cells[i];
        if (cell.category == CellCategory::EMPTY || cell.category == CellCategory::COOKIE || cell.category == CellCategory::SUPER_COOKIE)
        {
            continue;
        }

        sf::FloatRect blockBound = cell.bounds.getGlobalBounds();

        blockBound.top += tolerance;
        blockBound.left += tolerance;
        blockBound.width -= 2 * tolerance;
        blockBound.height -= 2 * tolerance;

        if (newBounds.intersects(blockBound))
        {
            if (offset.y < 0)
            {
                offset.y += blockBound.top + blockBound.height - newBounds.top;
            }
            else if (offset.y > 0)
            {
                offset.y -= newBounds.top + newBounds.height - blockBound.top;
            }
            if (offset.x < 0)
            {
                offset.x += blockBound.left + blockBound.width - newBounds.left;
            }
            else if (offset.x > 0)
            {
                offset.x -= newBounds.left + newBounds.width - blockBound.left;
            }
            changed = true;
            newBounds = moveRect(oldBounds, offset);
        }
    }

    return changed;
}