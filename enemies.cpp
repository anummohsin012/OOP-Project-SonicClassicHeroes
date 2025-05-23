#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Player.cpp"
using namespace sf;
class Enemy { //base class for enemies
protected:
    float x;
    float y;
    float speed; 
    int hp;
    bool alive;
    Texture texture;
    Sprite sprite;
    int width, height; 
    float hitboxl, hitboxt,hitboxr, hitboxb;
    //timer clockes with cooldown
    Clock damageclock; 
    float damagecooldown;
    bool damaged;

public:
    Enemy(float x, float y, float spd, int health, int w, int h ,float hitboxL, float hitboxT, float hitboxR, float hitboxB)
        : x(x), y(y), speed(spd), hp(health), alive(true), width(w), height(h), hitboxl(hitboxL), hitboxt(hitboxT), hitboxr(hitboxR), hitboxb(hitboxB),damagecooldown(1.0f) {
       
    }
    // folowing polynmorphism with pure virtayal functions making it an abstarct class
    
    virtual void updateposi(const Player* player, char** lvl, float cell_size) = 0;
       //to draww all enemies
    void draw(RenderWindow& window, float scrollOffsetX = 0) {
        if (alive) {
            sprite.setPosition(x - scrollOffsetX, y);
            window.draw(sprite);
        }
    }   //damage handling
        void takeDamage() {
            if (damageclock.getElapsedTime().asSeconds() >= damagecooldown) {
                hp -= 1;
                damaged = true;
                damageclock.restart();
                if (hp <= 0) {
                    alive = false;
                }
            }
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
    // tp check collisions betwwen enemies ans okayer
    static int checkcollision(PlayerManager& manager, Enemy* enemies[], int enemyCount) {
        float plax = manager.getLeader()->getXposition(); //by getting location of player through getters and setters 
        float play = manager.getLeader()->getYPosition();
        float plaw = 40.f * 2.5f;
        float plah = 40.f * 2.5f;
        int kills = 0;
        static Music collisionmusic;
        collisionmusic.openFromFile("Data/collision.ogg");
        collisionmusic.setVolume(60);
     
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i] && enemies[i]->isAlive()) 
            {
                float enemeyl = enemies[i]->getLeft();
                float enemeyr = enemies[i]->getRight();
                float enemeyt = enemies[i]->getTop();
                float enemeyb = enemies[i]->getBottom();
                if (plax < enemeyr && plax + plaw > enemeyl && play < enemeyb && play + plah > enemeyt) 
                {
                    collisionmusic.play();
                    if (!manager.getLeader()->isInvincible() &&  manager.getLeader()->getonground()) {
                        manager.removeLife(); }
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
    virtual ~Enemy() {} //destructor
};
//
class Crawler : public Enemy {
protected:
    //helper function to check how we turn around
    bool shouldTurnAround(char** lvl, float cell_size) {
        float fro = (speed > 0) ? getRight() + 1 : getLeft() - 1;
        float ycor = getBottom() - 5;
        int cellX = static_cast<int>(fro / cell_size);
        int cellY = static_cast<int>(ycor / cell_size);
        if (cellX < 0 || cellX >= width )
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
protected:
    bool shouldTurnAround(char** lvl, float cell_size) {
        float fro = (speed > 0) ? getRight() + 1 : getLeft() - 1;
        float ycor = getBottom() - 5;
        int cellX = static_cast<int>(fro / cell_size);
        int cellY = static_cast<int>(ycor / cell_size);
        if (cellX < 0 || cellX >= width || cellY < 0 || cellY >= height)
            return true;
        return (lvl[cellY][cellX] == 'w' || lvl[cellY][cellX] == 'v');
    }
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
    void movement() override {
        
    }
    void updateposi(const Player* player, char** lvl, float cell_size) override {
        if (shouldTurnAround(lvl, cell_size)) {
            speed = -speed;
            faceright = !faceright;
        }
        if (player->getXposition() < x) {
            x -= speed;
   
        }
        else {
            x += speed;

        }
        if (faceright == true) {
            texture.loadFromFile("Data/batbrain2.png");
            sprite.setTexture(texture);
            sprite.setScale(1.0f, 1.0f);
        }
        else {
            texture.loadFromFile("Data/batbrain3.png");
            sprite.setTexture(texture);
            sprite.setScale(1.0f, 1.0f);
        }
        sprite.setPosition(x, y);

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
    bool faceright;

public:
    BeeBot(float x, float y, int w, int h)
        : Flyer(x, y, 1.0f, 5, w, h), angle(0.0f), startY(y) {
        texture.loadFromFile("Data/beebot.png");
        sprite.setTexture(texture);
        sprite.setScale(1.0f, 1.0f);

        leftBound = x - 150;  
        hitboxl = 0;
        hitboxr = 100.0f;
        hitboxt = 0;
        hitboxb = 60.0f;
        
    }

    void movement() override {
        x += speed;
        if (x < leftBound )
            speed = -speed; 
        y = startY + sin(angle) * 40; 
        angle += 0.05f;
    }
    void updateposi(const Player* player, char** lvl, float cell_size) override {
        if (shouldTurnAround(lvl, cell_size)) {
            speed = -speed;
            faceright = !faceright;
        }
        x += speed;
        movement();
    }

};

class EggStinger : public Flyer {
private:
    bool goingdown;
    bool goingup;
    Clock stateClock;
    float idleTime;
    float descendSpeed;
    float ascendSpeed;
    float targetY;
    float targetX;
    SoundBuffer bf;
    Sound bs;

public:
    EggStinger(float x, float y, int w, int h)
        : Flyer(x, y, 1.0f, 20, w, h), goingdown(false), goingup(false), idleTime(10.0f), descendSpeed(5.0f), ascendSpeed(3.0f), targetY(y), targetX(x) {
        texture.loadFromFile("Data/boss.png");
        sprite.setTexture(texture);
        sprite.setScale(1.0f, 1.0f);
        hitboxl = 0;
        hitboxr = 148.0f;
        hitboxt = 0;
        hitboxb = 128.0f;
        stateClock.restart();
        if (!bf.loadFromFile("Data/Crumble.ogg")) {
            cout<< "Failed to load wall break sound effect!\n";
        }
        bs.setBuffer(bf);
        bs.setVolume(70);
    }

    void movement() override {}

    void updateposi(const Player* player, char** lvl, float cell_size) override {
        if (!goingdown && !goingup) {
            x += speed;
            if (x < 200 || x > width * cell_size - 200) {
                speed = -speed;
            }
            if (stateClock.getElapsedTime().asSeconds() >= idleTime) {
                targetX = player->getXposition();
                targetY = player->getYPosition();
                goingdown = true;
                stateClock.restart();
            }
        }
        else if (goingdown) {
            float dx = targetX - x;
            float dy = targetY - y;
            float distance = sqrt(dx * dx + dy * dy);
            if (distance > 0) {
                x += (dx / distance) * descendSpeed;
                y += (dy / distance) * descendSpeed;
            }
            if (distance < descendSpeed) {
                x = targetX;
                y = targetY;
                goingdown = false;
                goingup = true;
                stateClock.restart();
                int col = static_cast<int>((x + hitboxr/2) / cell_size);
                int row = static_cast<int>((y+hitboxb) / cell_size);
                if (row >= 0 && row < height && col >= 0 && col < width) {
                    cout << "yes" << endl;
                    if (lvl[row][col] == 'w' ) {
                        lvl[row][col] = 'p'; 
                        bs.play();
                    }
                }
            }
        }
        else if (goingup) {
            y -= ascendSpeed;
            if (y <= 250) {
                y = 250;
                goingup = false;
                stateClock.restart();
            }
        }
    }
};



class EnemyFactory {
public:
    //makes enemy based on it type and other paramters
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
