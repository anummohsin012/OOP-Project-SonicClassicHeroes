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
        void draw(RenderWindow& window, float scrollOffsetX = 0) {
        if (alive) {
            sprite.setPosition(x - scrollOffsetX, y);
            window.draw(sprite);

            //RectangleShape hitboxShape;
            //hitboxShape.setPosition(getLeft(), getTop());
            //hitboxShape.setSize(Vector2f(hitboxr - hitboxl, hitboxb - hitboxt));
            //hitboxShape.setFillColor(Color(255, 0, 0, 100)); // Semi-transparent red
            //window.draw(hitboxShape);
        }
    }
    void takeDamage() {
        hp -= 1;
        if (hp <= 0) 
            alive = false;
    }
    bool isAlive() const 
    {
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
    static int checkcollision(PlayerManager& manager, Enemy* enemies[], int enemyCount) {
        float px = manager.getLeader()->getXposition();
        float py = manager.getLeader()->getYPosition();
        float pw = 40.f * 2.5f;
        float ph = 40.f * 2.5f;
        int kills = 0;

        static Music collisionmusic;
        collisionmusic.openFromFile("Data/collision.ogg");
        collisionmusic.setVolume(60);
        

        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i] && enemies[i]->isAlive()) 
            {
                float ex1 = enemies[i]->getLeft();
                float ex2 = enemies[i]->getRight();
                float ey1 = enemies[i]->getTop();
                float ey2 = enemies[i]->getBottom();

                if (px < ex2 && px + pw > ex1 && py < ey2 && py + ph > ey1) 
                {
                    
                    collisionmusic.play();
                    if (!manager.getLeader()->isInvincible() &&  manager.getLeader()->getonground()) {
                        manager.removeLife();
                    }
                    else {
                        bool wasAlive = enemies[i]->isAlive();
                        enemies[i]->takeDamage();
                        if (wasAlive && !enemies[i]->isAlive()) 
                            kills++;
                    }
                }
            }
        }
        return kills;
        
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
protected:
    bool shouldTurnAround(char** lvl, float cell_size) {
        // Use absolute world coordinates (no scrolling adjustment needed)
        float checkFront = (speed > 0) ? getRight() + 1 : getLeft() - 1;
        float checkY = getBottom() - 5; // Check slightly above feet

        // Convert to grid coordinates
        int cellX = static_cast<int>(checkFront / cell_size);
        int cellY = static_cast<int>(checkY / cell_size);

        // Boundary check
        if (cellX < 0 || cellX >= width /*|| cellY < 0 || cellY >= height*/)
            return true;

        return (lvl[cellY][cellX] == 'w' || lvl[cellY][cellX] == 'v');
    }
public:
    Crawler(float x, float y, float spd, int hp, int w, int h) : Enemy(x, y, spd, hp, w, h,0,0,0,0) {}
    virtual void movement(char** lvl, float cell_size) = 0;
};

class Motobug : public Crawler {
private:
    bool faceright;
public:
    Motobug(float x, float y, int w, int h) : Crawler(x, y, 1.0f, 2, w, h), faceright(false) {
        texture.loadFromFile("Data/motobugright.png");
        sprite.setTexture(texture);
        sprite.setScale(2.0f, 2.0f);
        hitboxl = 0;       
        hitboxr = 80.0f;   
        hitboxt = 0;       
        hitboxb = 60.0f;
    }
    void movement(char** lvl, float cell_size) override 
    {
        if (shouldTurnAround(lvl, cell_size)) {
            speed = -speed;
            faceright = !faceright;
        }

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

        hitboxl = 0;       
        hitboxr = 100.0f;   
        hitboxt = 0;       
        hitboxb = 61.0f;
    }
    void movement(char** lvl, float cell_size) override {
        if (shouldTurnAround(lvl, cell_size)) {
            speed = -speed;
        }

        x += speed;
    }
    void updateposi(const Player* player, char** lvl, float cell_size) override {
        movement(lvl, cell_size);
      
    }

 
    

};



class Flyer : public Enemy {
public:
    Flyer(float x, float y, float spd, int hp, int w, int h) : Enemy(x, y, spd, hp, w, h,0,0,0,0) {}
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

        hitboxl = 0;    
        hitboxr = 100.0f;   
        hitboxt = 0;         
        hitboxb = 105.0f;
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

};

class EggStinger : public Flyer {
private:
    float timer;
    Clock clock;
    bool goingdown;
    float gdtime;
    float xx;
    float yy;
public:
    EggStinger(float x, float y, int w, int h): Flyer(x, y, 1.0f, 20, w, h),timer(0),goingdown(false),gdtime(10.0f),yy(6){
        texture.loadFromFile("Data/boss.png");
        sprite.setTexture(texture);
        sprite.setScale(1.0f, 1.0f);

    }
    void movement() override {
        x += speed;
        if (x < 200 || x > width * 64 - 200)
            speed = -speed;
        float elapsed = clock.getElapsedTime().asSeconds();
        if (elapsed >= gdtime) {
            clock.restart();
            goingdown= true;
            xx = x;
        }
        while (goingdown) {
            y += 5.0f;
            if (y >= yy) {
                y =yy;
                goingdown = false;
            }
        }
    }
    void updateposi(const Player* player, char** lvl, float cell_size) override {
        movement();
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
        else if (type == 4)
            return new EggStinger(x, y, width, height);
        else
            return nullptr;
    }
};

