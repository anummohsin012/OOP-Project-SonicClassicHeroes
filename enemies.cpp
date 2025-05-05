#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace sf;
class Enemy {
protected:
    int hp;
    Texture texture;
    Sprite sprite;
    float speedX, speedY;
    int gridWidth, gridHeight;

public:
    Enemy(int hp, Texture& texture, float x, float y, float speedX, float speedY, int gridW, int gridH)
        : hp(hp), texture(texture), speedX(speedX), speedY(speedY), gridWidth(gridW), gridHeight(gridH) {
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    virtual void update(float deltaTime, float playerX, float playerY) = 0;
    virtual void draw(RenderWindow& window) = 0;
    virtual void uniqueBehavior(float deltaTime) = 0;
    Sprite& getSprite() { return sprite; }
    void takeDamage(int damage) { hp -= damage; }
    bool isDead() const { return hp <= 0; }

    void checkBounds() {
        float x = sprite.getPosition().x;
        float y = sprite.getPosition().y;
        float w = (float)texture.getSize().x;
        float h = (float)texture.getSize().y;
        if (x <= 0 || x + w >= gridWidth) speedX *= -1;
        if (y <= 0 || y + h >= gridHeight) speedY *= -1;
    }

    virtual ~Enemy() = default;
};

class Crawler : public Enemy {
public:
    Crawler(int hp,Texture& texture, float x, float y, float speedX, int gridW, int gridH)
        : Enemy(hp, texture, x, y, speedX, 0.f, gridW, gridH) {
    }

    void update(float deltaTime, float playerX, float playerY) override {
        sprite.move(speedX * deltaTime, 0);
        checkBounds();
    }

    void draw(RenderWindow& window) override {
        window.draw(sprite);
    }

    void uniqueBehavior(float deltaTime) override {
    }
};


class Flyer : public Enemy {
public:
    Flyer(int hp, sf::Texture& texture, float x, float y, float speedX, float speedY, int gridW, int gridH)
        : Enemy(hp, texture, x, y, speedX, speedY, gridW, gridH) {
    }

    void update(float deltaTime, float playerX, float playerY) override {
        sprite.move(speedX * deltaTime, speedY * deltaTime);
        checkBounds();
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }

    void uniqueBehavior(float deltaTime) override {
    }
};

// Motobug class (inherits from Crawler)
class Motobug : public Crawler {
private:
    float playerX, playerY;

public:
    Motobug(sf::Texture& texture, float x, float y, int gridW, int gridH)
        : Crawler(2, texture, x, y, 50.f, gridW, gridH) {
    }

    void update(float deltaTime, float playerXin, float playerYin) override {
        playerX = playerXin;  // Store player's position
        playerY = playerYin;

        // Follow player horizontally
        if (sprite.getPosition().x < playerX) {
            sprite.move(speedX * deltaTime, 0);  // Move right
        }
        else if (sprite.getPosition().x > playerX) {
            sprite.move(-speedX * deltaTime, 0);  // Move left
        }

        checkBounds();
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }

    void uniqueBehavior(float deltaTime) override {
        
    }
};

// BeeBot class (inherits from Flyer)
class BeeBot : public Flyer {
private:
    bool goingUp = true;
    float zigzagTime = 0;

public:
    BeeBot(sf::Texture& texture, float x, float y, int gridW, int gridH)
        : Flyer(5, texture, x, y, 40.f, 40.f, gridW, gridH) {
    }

    void update(float deltaTime, float playerX, float playerY) override {
        sprite.move(speedX * deltaTime, speedY * deltaTime);
        checkBounds();
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }

    void uniqueBehavior(float deltaTime) override {
        // BeeBot's zigzag movement
        zigzagTime += deltaTime;
        if (zigzagTime > 0.5f) {
            speedX = -speedX;  // Change direction every 0.5 seconds
            zigzagTime = 0;
        }
    }
};

// BatBrain class (inherits from Flyer)
class BatBrain : public Flyer {
private:
    float playerX, playerY;

public:
    BatBrain(sf::Texture& texture, float x, float y, int gridW, int gridH)
        : Flyer(3, texture, x, y, 30.f, 0.f, gridW, gridH) {
    }

    void update(float deltaTime, float playerX, float playerY) override {
        this->playerX = playerX;
        this->playerY = playerY;
        sprite.move(speedX * deltaTime, 0);
        checkBounds();
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }

    void uniqueBehavior(float deltaTime) override {
        // BatBrain follows the player vertically
        if (sprite.getPosition().y < playerY) {
            sprite.move(0, speedY * deltaTime);  // Move down
        }
        else if (sprite.getPosition().y > playerY) {
            sprite.move(0, -speedY * deltaTime);  // Move up
        }
    }
};

// EggStinger class (inherits from Flyer)
class EggStinger : public Flyer {
public:
    bool goingDown = true;

    EggStinger(sf::Texture& texture, float x, float y, int gridW, int gridH)
        : Flyer(15, texture, x, y, 70.f, 60.f, gridW, gridH) {
    }

    void update(float deltaTime, float playerX, float playerY) override {
        sprite.move(speedX * deltaTime, speedY * deltaTime);
        checkBounds();
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }

    void uniqueBehavior(float deltaTime) override {
        // EggStinger's unique behavior: Alternates moving up and down in the air
        if (goingDown) {
            sprite.move(0, speedY * deltaTime);
            if (sprite.getPosition().y >= gridHeight - 100) goingDown = false;
        }
        else {
            sprite.move(0, -speedY * deltaTime);
            if (sprite.getPosition().y <= 0) goingDown = true;
        }
    }
};

// Factory for creating enemies
class EnemyFactory {
public:
    static Enemy* createEnemy(const std::string& type, sf::Texture& texture, float x, float y, int gridW, int gridH) {
        if (type == "Motobug") {
            return new Motobug(texture, x, y, gridW, gridH);
        }
        else if (type == "BeeBot") {
            return new BeeBot(texture, x, y, gridW, gridH);
        }
        else if (type == "BatBrain") {
            return new BatBrain(texture, x, y, gridW, gridH);
        }
        else if (type == "EggStinger") {
            return new EggStinger(texture, x, y, gridW, gridH);
        }
        return nullptr;
    }
};

// Level class for managing enemies
class Level {
private:
    int levelNumber;
    std::vector<Enemy*> enemies;

public:
    Level(int levelNumber) : levelNumber(levelNumber) {}

    void spawnEnemies(sf::Texture& motobugTexture, sf::Texture& beeBotTexture, sf::Texture& batBrainTexture, sf::Texture& eggStingerTexture, int gridWidth, int gridHeight) {
        int numMotobugs = levelNumber * 2;
        int numBeeBots = levelNumber;
        int numBatBrains = levelNumber;
        int numEggStingers = 1;

        srand(time(NULL));

        for (int i = 0; i < numMotobugs; ++i) {
            spawnEnemy("Motobug", motobugTexture, gridWidth, gridHeight);
        }
        for (int i = 0; i < numBeeBots; ++i) {
            spawnEnemy("BeeBot", beeBotTexture, gridWidth, gridHeight);
        }
        for (int i = 0; i < numBatBrains; ++i) {
            spawnEnemy("BatBrain", batBrainTexture, gridWidth, gridHeight);
        }
        for (int i = 0; i < numEggStingers; ++i) {
            spawnEnemy("EggStinger", eggStingerTexture, gridWidth, gridHeight);
        }
    }

    void spawnEnemy(const std::string& type, sf::Texture& texture, int gridWidth, int gridHeight) {
        float x = rand() % (gridWidth - 100);
        float y = rand() % (gridHeight - 100);

        // Ensure no overlap
        for (Enemy* enemy : enemies) {
            if (fabs(enemy->getSprite().getPosition().x - x) < 50 && fabs(enemy->getSprite().getPosition().y - y) < 50) {
                x = rand() % (gridWidth - 100);
                y = rand() % (gridHeight - 100);
            }
        }

        Enemy* newEnemy = EnemyFactory::createEnemy(type, texture, x, y, gridWidth, gridHeight);
        enemies.push_back(newEnemy);
    }

    std::vector<Enemy*>& getEnemies() { return enemies; }
};

int main() {
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Sonic Game");

    // Load textures
    sf::Texture motobugTexture, beeBotTexture, batBrainTexture, eggStingerTexture;
    motobugTexture.loadFromFile("Data/motobug.png");
    beeBotTexture.loadFromFile("beebot.png");
    batBrainTexture.loadFromFile("batbrain.png");
    eggStingerTexture.loadFromFile("eggstinger.png");

    Level level(1);  // Level number can change dynamically
    level.spawnEnemies(motobugTexture, beeBotTexture, batBrainTexture, eggStingerTexture, WINDOW_WIDTH, WINDOW_HEIGHT);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();

        // Update and draw all enemies
        for (Enemy* enemy : level.getEnemies()) {
            enemy->update(0.1f, 400.f, 300.f);  // Dummy player position
            enemy->uniqueBehavior(0.1f);  // Unique behavior for each enemy
            enemy->draw(window);
        }

        window.display();
    }

    return 0;
}
