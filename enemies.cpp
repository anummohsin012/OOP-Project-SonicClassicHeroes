#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Player.cpp"
using namespace sf;
class Enemy {
protected:
    float x, y;
    float speed;
    int hp;
    bool alive;
    Texture texture;
    Sprite sprite;
    int width, height;
   float hitboxl, hitboxt,hitboxr, hitboxb;
   


public:
    Enemy(float x, float y, float spd, int health, int w, int h ,float hitboxL, float hitboxT, float hitboxR, float hitboxB)
        : x(x), y(y), speed(spd), hp(health), alive(true), width(w), height(h), hitboxl(hitboxL), hitboxt(hitboxT), hitboxr(hitboxR), hitboxb(hitboxB) {
       
    }

    virtual void updateposi(const Player* player, char** lvl, float cell_size) = 0;
    virtual void draw(sf::RenderWindow& window) {
        if (alive) {
            sprite.setPosition(x - 20, y - 20);
            window.draw(sprite);

            RectangleShape hitboxShape;
            hitboxShape.setPosition(getLeft(), getTop());
            hitboxShape.setSize(Vector2f(hitboxr - hitboxl, hitboxb - hitboxt));
            hitboxShape.setFillColor(Color(255, 0, 0, 100)); // Semi-transparent red
            window.draw(hitboxShape);
        }
    }
    void takeDamage() {
        hp -= 1;
        if (hp <= 0) alive = false;
    }
    bool isAlive() const { 
        return alive;
    }
    float getLeft() const { 
        return x + hitboxl;
    }
    float getRight() const {
        return x + hitboxr; 
    }
    float getTop() const {
        return y + hitboxt; 
    }
    float getBottom() const { 
        return y + hitboxb; 
    }
    static void checkcollision(PlayerManager& manager, Enemy* enemies[], int enemyCount) {
        float px = manager.getLeader()->getXposition();
        float py = manager.getLeader()->getYPosition();
        float pw = 40.f * 2.5f;
        float ph = 40.f * 2.5f;

        static Music collisionmusic;
        collisionmusic.openFromFile("Data/collision.ogg");
        collisionmusic.setVolume(60);
        

        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i] && enemies[i]->isAlive()) {
                float ex1 = enemies[i]->getLeft();
                float ex2 = enemies[i]->getRight();
                float ey1 = enemies[i]->getTop();
                float ey2 = enemies[i]->getBottom();

                if (px < ex2 && px + pw > ex1 && py < ey2 && py + ph > ey1) {
                    enemies[i]->takeDamage();
                    collisionmusic.play();
                    if (!manager.getLeader()->isInvincible() &&  manager.getLeader()->getonground()) {
                        manager.removeLife();
                        

                    }
                }
            }
        }
 
        
    }
    virtual ~Enemy() {}
};


class Projectile {
private:
    float x, y;
    float speedX, speedY;
    bool active;
    Texture texture;
    Sprite sprite;

public:
    Projectile(float startX, float startY, float dirX, float dirY)
        : x(startX), y(startY), speedX(dirX), speedY(dirY), active(true) {
        texture.loadFromFile("Data/life.png");
        sprite.setTexture(texture);
        sprite.setScale(1.0f, 1.0f);
        sprite.setPosition(x, y);
    }
    void update() {
        x += speedX;
        y += speedY;
        sprite.setPosition(x, y);
    }
    void draw(sf::RenderWindow& window) {
        if (active)
            window.draw(sprite);
    }
    bool isActive() const { return active; }
    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return 20.f; } 
    float getHeight() const { return 20.f; }
};



class Crawler : public Enemy {
public:
    Crawler(float x, float y, float spd, int hp, int w, int h) : Enemy(x, y, spd, hp, w, h,-20,-20,60,120) {}
    virtual void movement(char** lvl, float cell_size) = 0;
};

class Motobug : public Crawler {
private:
    bool faceright;
public:
    Motobug(float x, float y, int w, int h) : Crawler(x, y, 1.0f, 2, w, h), faceright(false) {
        texture.loadFromFile("Data/motobugright.png");
        sprite.setTexture(texture);
        sprite.setScale(1.0f, 1.0f);
    }
    void movement(char** lvl, float cell_size) override {
        x += speed;
        if (faceright == true) {
            texture.loadFromFile("Data/motobug.png");
            sprite.setTexture(texture);
            sprite.setScale(2.0f, 2.0f);
        }
        else {
            texture.loadFromFile("Data/motobugright.png");
            sprite.setTexture(texture);
            sprite.setScale(2.0f, 2.0f);
        }
        sprite.setPosition(x, y);
    }
    void updateposi(const Player* player, char** lvl, float cell_size) override {
        if (player->getXposition() < x && speed > 0) { speed = -speed; 
        faceright = false;
        }
        else if (player->getXposition() > x && speed < 0) { speed = -speed; 
        faceright = true;
        }
        movement(lvl, cell_size);
    }
};

class CrabMeat : public Crawler {
    
public:
    CrabMeat(float x, float y, int w, int h) : Crawler(x, y, 1.0f, 4, w, h){
        texture.loadFromFile("Data/crabmeat2.png");
        sprite.setTexture(texture);
        sprite.setScale(1.0f, 1.0f);
    }
    void movement(char** lvl, float cell_size) override {
        x += speed;
        if (x <= 0 || x >= width * cell_size) speed *= -1;
    }
    void updateposi(const Player* player, char** lvl, float cell_size) override {
        movement(lvl, cell_size);
      
    }
    void draw(sf::RenderWindow& window) override {
        Enemy::draw(window);
        
    }
 
    

};



class Flyer : public Enemy {
public:
    Flyer(float x, float y, float spd, int hp, int w, int h) : Enemy(x, y, spd, hp, w, h,-20,-20,100,60) {}
    virtual void movement() = 0;
};

class BatBrain : public Flyer {
private:
    bool faceright;
public:
    BatBrain(float x, float y, int w, int h) : Flyer(x, y, 0.5f, 3, w, h) {
        texture.loadFromFile("Data/batbrain2.png");
        sprite.setTexture(texture);
        sprite.setScale(1.0f, 1.0f);
    }
    void movement() override {}
    void updateposi(const Player* player, char** lvl, float cell_size) override {
        if (player->getXposition() < x) {
            x -= speed;
            texture.loadFromFile("Data/batbrain2.png");
            sprite.setTexture(texture);
            sprite.setScale(1.0f, 1.0f);
        }
        else {
            x += speed;
            texture.loadFromFile("Data/batbrain3.png");
            sprite.setTexture(texture);
            sprite.setScale(1.0f, 1.0f);

        }

        if (player->getYPosition() < y) {
            y -= speed;
        }
        else {
            y += speed;
        }
    }
};

class BeeBot : public Flyer {
private:
    float angle;
    float startY;
    float leftBound, rightBound;

public:
    BeeBot(float x, float y, int w, int h)
        : Flyer(x, y, 1.0f, 5, w, h), angle(0.0f), startY(y) {
        texture.loadFromFile("Data/beebot.png");
        sprite.setTexture(texture);
        sprite.setScale(1.0f, 1.0f);

        leftBound = x - 150;  
        
    }

    void movement() override {
        x += speed;

        if (x < leftBound )
            speed = -speed; 

        y = startY + sin(angle) * 40; 
        angle += 0.05f;
    }
    void updateposi(const Player* player, char** lvl, float cell_size) override {
        x += speed;
        movement();
    }
    void draw(RenderWindow& window) override {
        Enemy::draw(window);
        
    }
};
class EnemyFactory {
public:
    static Enemy* createEnemy(int type, float x, float y, int width, int height) {
        if (type == 0)
            return new Motobug(x, y, width, height);
        else if (type == 1)
            return new CrabMeat(x, y, width, height);
        else if (type == 2)
            return new BatBrain(x, y, width, height);
        else if (type == 3)
            return new BeeBot(x, y, width, height);
        else
            return nullptr;
    }
};

int main() {
    const int height = 14, width = 200, cell_size = 64;

    char** lvl = new char* [height];
    for (int i = 0; i < height; ++i) {
        lvl[i] = new char[width];
        for (int j = 0; j < width; ++j)
            lvl[i][j] = ' ';
    }
    for (int j = 0; j < width; ++j)
        lvl[13][j] = 'w'; 

    RenderWindow window(VideoMode(1280, 720), "Sonic Classic Heroes Test");
    window.setFramerateLimit(60);

    PlayerFactory factory;
    PlayerManager manager(factory, lvl, height, width,10,1);

    // Setup enemies
    Enemy* enemies[4];
    enemies[0] = EnemyFactory::createEnemy(0, 500, 700, width, height);  // Motobug
    enemies[1] = EnemyFactory::createEnemy(1, 900, 700, width, height);  // CrabMeat
    enemies[2] = EnemyFactory::createEnemy(2, 1300, 500, width, height); // BatBrain
    enemies[3] = EnemyFactory::createEnemy(3, 700, 200, width, height); // BeeBot

    Event ev;

    while (window.isOpen()) {
        while (window.pollEvent(ev)) {
            if (ev.type == Event::Closed)
                window.close();
            if (ev.type == Event::KeyPressed) {
                if (ev.key.code == Keyboard::Tab)
                    manager.changePlayer(); // Switch character
            }
        }

        // Input for leader only
        if (Keyboard::isKeyPressed(Keyboard::Right))
            manager.getLeader()->moveRight(lvl, cell_size);
        else if (Keyboard::isKeyPressed(Keyboard::Left))
            manager.getLeader()->moveLeft(lvl, cell_size);
        else
            manager.getLeader()->setVelocityX(0); // Stop if no key pressed

        if (Keyboard::isKeyPressed(Keyboard::Space))
            manager.getLeader()->jump();

        if (Keyboard::isKeyPressed(Keyboard::LShift))
            manager.getLeader()->useSpecialAbility();

        // Update physics and ground collision
        for (int i = 0; i < 3; ++i) {
            manager.getPlayer(i)->updatePhysics();
            if (manager.getPlayer(i)->getYPosition() >= 600) {
                manager.getPlayer(i)->setYPosition(600);
                manager.getPlayer(i)->setVelocityY(0);
                manager.getPlayer(i)->setOnGround(true);
            }
        }

        // Update enemy AI
        for (int i = 0; i < 4; ++i)
            if (enemies[i] && enemies[i]->isAlive())
                enemies[i]->updateposi(manager.getLeader(), lvl, (float)cell_size);

        // Update followers logic
        manager.updateFollowers();
        manager.checkPits();

        Enemy::checkcollision(manager, enemies, 4);


        // Rendering
        window.clear(Color::Black);

        for (int i = 0; i < 3; ++i) {
            const Sprite& sprite = manager.getPlayer(i)->getSprite();
            Sprite s = sprite;
            s.setPosition(manager.getPlayer(i)->getXposition(), manager.getPlayer(i)->getYPosition());
            window.draw(s);
        }

        for (int i = 0; i < 4; ++i)
            if (enemies[i] && enemies[i]->isAlive())
                enemies[i]->draw(window);


        CircleShape dummyCircle(15);
        dummyCircle.setFillColor(Color::Cyan);
        dummyCircle.setPosition(manager.getLeader()->getXposition(), manager.getLeader()->getYPosition());
        window.draw(dummyCircle);

        window.display();
    }

    // Cleanup
    for (int i = 0; i < 4; ++i)
        delete enemies[i];
    for (int i = 0; i < height; ++i)
        delete[] lvl[i];
    delete[] lvl;

    return 0;
}





