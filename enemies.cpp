//#include <SFML/Graphics.hpp>
//#include <cstdlib>
//#include <ctime>
//
//using namespace sf;
//
//
//class Enemy {
//protected:
//    Sprite sprite;
//    Texture tex;
//    int hp;
//    float speedX;
//    float speedY;
//    int wid;
//    int hei;
//    bool isact;
//public:
//    Enemy(int hp, Texture& texture, float x, float y, float speedX, float speedY, int gridW, int gridH) : hp(hp), tex(texture), speedX(speedX), speedY(speedY), wid(gridW), hei(gridH), isact(true) {
//        sprite.setTextureRect(IntRect(0, 0, 40, 40));
//        sprite.setTexture(texture);
//        sprite.setPosition(x, y);
//    }
//    virtual void update(float deltaTime, float playerX, float playerY) = 0;
//    virtual void draw(RenderWindow& window) = 0;
//    virtual void uniqueBehavior(float deltaTime) = 0;
//
//    Sprite& getSprite() {
//        return sprite;
//    }
//
//    void takeDamage(int damage) {
//        hp -= damage;
//        if (hp <= 0) {
//            isact = false;
//        }
//    }
//
//    bool isDead() const {
//        return hp <= 0;
//    }
//    bool getActiveStatus() const {
//        return isact;
//    }
//
//    void checkBounds() {
//        float x = sprite.getPosition().x;
//        float y = sprite.getPosition().y;
//        float w = (float)tex.getSize().x;
//        float h = (float)tex.getSize().y;
//        
//        if (y <= 0 || y + h >= hei) speedY *= -1;
//    }
//};
//
//
//class Crawler : public Enemy {
//public:
//    Crawler(int hp, Texture& texture, float x, float y, float speedX, int gridW, int gridH)
//        : Enemy(hp, texture, x, y, speedX,speedY, gridW, gridH) {
//    }
//
//    void update(float deltaTime, float playerX, float playerY) override {
//        sprite.move(speedX * deltaTime, 0);
//        checkBounds();
//    }
//
//    void draw(RenderWindow& window) override {
//        sprite.setTextureRect(IntRect(0, 0, 40, 40));
//        window.draw(sprite);
//    }
//    void uniqueBehavior(float deltaTime) override {}
//
//};
//
//class Flyer : public Enemy {
//public:
//    Flyer(int hp, Texture& texture, float x, float y, float speedX, float speedY, int gridW, int gridH)
//        : Enemy(hp, texture, x, y, speedX, speedY, gridW, gridH) {
//    }
//
//    void update(float deltaTime, float playerX, float playerY) override {
//        sprite.move(speedX * deltaTime, speedY * deltaTime);
//        checkBounds();
//    }
//
//    void draw(RenderWindow& window) override {
//        sprite.setTextureRect(IntRect(0, 0, 40, 40));
//        window.draw(sprite);
//    }
//
//    void uniqueBehavior(float deltaTime) override {}
//
//};
//
//class BatBrain : public Flyer {
//public:
//    BatBrain(Texture& texture, float x, float y, int gridW, int gridH)
//        : Flyer(3, texture, x, y, 80.f, 80.f, gridW, gridH) {
//    }
//    void update(float dt, float playerX, float playerY) override {
//        Vector2f pos = sprite.getPosition();
//        if (pos.x < playerX) pos.x += 30 * dt;
//        else pos.x -= 30 * dt;
//        if (pos.y < playerY) pos.y += 15 * dt;
//        else pos.y -= 15 * dt;
//        sprite.setPosition(pos);
//        checkBounds();
//    }
//    void draw(RenderWindow& window) override {
//        sprite.setTextureRect(IntRect(80, 0, 40, 40)); 
//        window.draw(sprite);
//    }
//    void uniqueBehavior(float) override {
//       
//    }
//
//};
//
//class BeeBot : public Enemy {
//private:
//    float zigzagTimer = 0.f;
//    float zigzagInterval = 0.5f;  // Switch X direction every 0.5 seconds
//    int zigzagDirection = 1;
//
//    float shootCooldown = 5.f;   // Shoots every 5 seconds
//    float timeSinceLastShot = 0.f;
//
//public:
//    BeeBot(Texture& texture, float x, float y, int gridW, int gridH)
//        : Enemy(5, texture, x, y, 60.f, 40.f, gridW, gridH) {
//    }
//
//    void update(float dt, float playerX, float playerY) override {
//        // Zigzag movement logic
//        zigzagTimer += dt;
//        if (zigzagTimer >= zigzagInterval) {
//            zigzagDirection *= -1;
//            zigzagTimer = 0.f;
//        }
//
//        sprite.move(speedX * zigzagDirection * dt, speedY * dt);
//
//        // Shooting logic
//        timeSinceLastShot += dt;
//        if (timeSinceLastShot >= shootCooldown) {
//            shoot();
//            timeSinceLastShot = 0.f;
//        }
//
//        checkBounds();
//    }
//
//    void draw(RenderWindow& window) override {
//        sprite.setTextureRect(IntRect(0, 0, 40, 40));  // Adjust as needed
//        window.draw(sprite);
//    }
//
//    void uniqueBehavior(float dt) override {
//        
//    }
//
//    void shoot() {
//
//    }
//};
//
//
//
//class Motobug : public Crawler {
//public:
//    Motobug(Texture& texture, float x, float y, int gridW, int gridH) : Crawler(2, texture, x, y, 50.f, gridW, gridH) {}
//
//
//    void update(float deltaTime, float playerX, float playerY) override {
//        float myX = sprite.getPosition().x;
//        float playerDir = (playerX > myX) ? 1.f : -1.f;
//
//        sprite.move(playerDir * speedX * deltaTime, 0.f);  // Only move in X
//        checkBounds();
//    }
//
//    void draw(RenderWindow& window) override {
//        sprite.setTextureRect(IntRect(0, 0, 40, 40)); // Optional visual slice
//        window.draw(sprite);
//    }
//
//    void uniqueBehavior(float deltaTime) override {
//        // Not used for Motobug
//    }
//
//};
//
//class CrabMeat : public Crawler {
//private:
//    float shootTimer = 0.f;
//    float shootCooldown = 5.f;  // Shoot every 5 seconds
//
//    float projX = -100;   // Offscreen by default
//    float projY = -100;
//    bool projActive = false;
//
//public:
//    CrabMeat(Texture& texture, float x, float y, int gridW, int gridH)
//        : Crawler(4, texture, x, y, 60.f,  gridW, gridH) {
//    }
//
//    void update(float deltaTime, float playerX, float playerY) override {
//        sprite.move(speedX * deltaTime, 0.f);
//        checkBounds();
//
//        // Shoot timer
//        shootTimer += deltaTime;
//        if (shootTimer >= shootCooldown) {
//            shootProjectile();
//            shootTimer = 0.f;
//        }
//
//        // Update projectile if active
//        if (projActive) {
//            projX += 200 * deltaTime;
//            if (projX > wid) {
//                projActive = false;
//            }
//        }
//    }
//
//    void draw(RenderWindow& window) override {
//        sprite.setTextureRect(IntRect(0, 0, 40, 40));
//        window.draw(sprite);
//
//        // Draw projectile if active
//        if (projActive) {
//            RectangleShape bullet(Vector2f(10, 4));
//            bullet.setFillColor(Color::Red);
//            bullet.setPosition(projX, projY);
//            window.draw(bullet);
//        }
//    }
//
//    void uniqueBehavior(float deltaTime) override {}
//
//    void shootProjectile() {
//        projX = sprite.getPosition().x + 40;
//        projY = sprite.getPosition().y + 15;
//        projActive = true;
//    }
//};
//
//class EggStinger : public Enemy {
//private:
//    float diveTimer = 0.f;
//    float diveInterval = 10.f;
//    bool diving = false;
//
//public:
//    EggStinger(Texture& texture, float x, float y, int gridW, int gridH)
//        : Enemy(15, texture, x, y, 60.f, 0.f, gridW, gridH) {
//    }
//
//    void update(float dt, float playerX, float playerY) override {
//        diveTimer += dt;
//
//        if (!diving)
//            sprite.move(speedX * dt, 0.f);
//
//        if (diveTimer >= diveInterval) {
//            sprite.setPosition(playerX, sprite.getPosition().y); // align X
//            sprite.move(0.f, 80.f); // dive down (simulate destroying ground)
//            diving = true;
//            diveTimer = 0.f;
//        }
//        else if (diving) {
//            sprite.move(0.f, -80.f * dt); // return up slowly
//            if (sprite.getPosition().y <= 100.f) { // assume original Y
//                sprite.setPosition(sprite.getPosition().x, 100.f);
//                diving = false;
//            }
//        }
//
//        checkBounds();
//    }
//
//    void draw(RenderWindow& window) override {
//        sprite.setTextureRect(IntRect(0, 0, 40, 40));
//        window.draw(sprite);
//    }
//
//    void uniqueBehavior(float dt) override {}
//};
//
//
//
//class EnemyFactory {
//public:
//    static Enemy* createEnemy(const std::string& type, Texture& texture, float x, float y, int gridW, int gridH) {
//        if (type == "Crawler") {
//            return new Crawler(2, texture, x, y, 60.f, gridW, gridH);
//        }
//        else if (type == "Flyer") {
//            return new Flyer(3, texture, x, y, 80.f, 80.f, gridW, gridH);
//        }
//        else if (type == "BatBrain") {
//            return new BatBrain(texture, x, y, gridW, gridH);
//        }
//        else if (type == "BeeBot") {
//            return new BeeBot(texture, x, y, gridW, gridH);
//        }
//        else if (type == "Motobug") {
//            return new Motobug(texture, x, y, gridW, gridH);
//        }
//        else if (type == "CrabMeat") {
//            return new CrabMeat(texture, x, y, gridW, gridH);
//        }
//
//        return nullptr; // Default fallback
//    }
//};
//
//
//int main() {
//    sf::RenderWindow window(sf::VideoMode(800, 600), "Sonic Classic Heroes");
//
//    // Load textures
//    sf::Texture motobugTexture, beeBotTexture, batBrainTexture, eggStingerTexture;
//    if ((!motobugTexture.loadFromFile("Data/motobug.png")) ||
//        (!beeBotTexture.loadFromFile("Data/beebot.png")) ||
//        (!batBrainTexture.loadFromFile("Data/BatBrain.png"))){}
//    float playerX = 400.f;  // placeholder
//    float playerY = 300.f;  // placeholder
//    Texture bossTex;
//    bossTex.loadFromFile("Data/boss.png");
//    Enemy* boss = new EggStinger(bossTex, 300, 100, 800, 600);
//
//
//    // Create enemies
//    Enemy* motobug = EnemyFactory::createEnemy("Motobug", motobugTexture, playerX, playerY,800,600);
//    Enemy* beebot = EnemyFactory::createEnemy("BeeBot", beeBotTexture, playerX, playerY, 800, 600);
//   
//    Enemy* batbrain = EnemyFactory::createEnemy("BatBrain", batBrainTexture, playerX, playerY, 800, 600);
//   
//
//    sf::Clock clock;
//
//    while (window.isOpen()) {
//        sf::Time deltaTime = clock.restart();
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed)
//                window.close();
//        }
//        playerX += 50 * deltaTime.asSeconds();
//        playerY -= 30 * deltaTime.asSeconds();playerX -= 10;
//        
//        playerX-=20* deltaTime.asSeconds();
//       
//
//        // Update
//        motobug->update(deltaTime.asSeconds(), playerX, playerY);
//        beebot->update(deltaTime.asSeconds(), playerX, playerY);
//        batbrain->update(deltaTime.asSeconds(), playerX, playerY);
//        boss->update(deltaTime.asSeconds(), playerX, playerY);
//
//       
//        // Render
//        window.clear();
//        motobug->draw(window);
//        beebot->draw(window);
//        batbrain->draw(window);
//        boss->draw(window);
//        
//        window.display();
//    }
//
//    delete motobug;
//    delete beebot;
//    delete batbrain;
//   
//
//    return 0;
//}