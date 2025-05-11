#include "player.cpp"
#include "collectables.cpp"
#include "timer.cpp"
#include "enemies.cpp"

using namespace std;
using namespace sf;


class Levels {
protected:
	const int cell_size;
	const int height;
	const int width;
	char** lvl;
    CollectibleFactory factory; //composed the factory as the levels have a factory of collectibles
    PlayerFactory playerfactory; //each level has a factory that makes players
    PlayerManager manager; //each level has a mnagaer that uses the factory
	Texture lvlTex1;
    Texture lvlTex2;
    Texture lvlTex3;
    Texture bossleveltex;
	Sprite lvlSprite;
	Music lvlMus;
	int rings;
	int ringscollected;
	int score;
	int exlives;
    Texture wall; 
    Sprite wallSprite;
    Texture spikes;
    Sprite spikesSprite;
    Text livesText;
    Text ringsText;
    Font font;
    Texture wall2;
    Sprite wall2Sprite;
    Timer *lvltimer;
    Text timerText;
    float lvlTime;
    Texture dooropen;
    Texture doorclosed;
    Sprite door;
    int enemyCapacity;
    int enemyCount;
    Enemy** enemies;
    Text scoreText;
    bool lvlFinished;
    Text levelIntroText;
    Clock levelIntroClock;
    bool showLevelIntro;

public:
	Levels(int h, int w, int rings, int e_l, float a, float g,float duration, Timer& timer)
		:height(h), width(w), cell_size(64), lvl(new char* [height]), manager(playerfactory,lvl, height, width,a, g), rings(0), ringscollected(0), score(0), exlives(e_l), factory(rings+e_l), lvlTime(duration),lvltimer(&timer)
        ,enemies(new Enemy*[enemyCapacity]), enemyCount(0), enemyCapacity(3), lvlFinished(false), showLevelIntro(true)
	{
        for (int i = 0; i < height; i ++) 
        {
            lvl[i] = new char[width];
            for (int j = 0; j < width; j ++) 
            {
                lvl[i][j] = ' '; // Initialize all cells to empty
            }
        }
        if (!lvlTex1.loadFromFile("Data/bglvl1.png"))
        {
            return;
        }
        lvlTex1.setRepeated(true);

        if (!lvlTex2.loadFromFile("Data/bglvl2.png"))
        {
            return;
        }
        lvlTex2.setRepeated(true);

        if (!lvlTex3.loadFromFile("Data/bglvl3.png"))
        {
            return;
        }
        lvlTex3.setRepeated(true);
        if (!bossleveltex.loadFromFile("Data/bg1.png"))
        {
            return;
        }
        


        wall.loadFromFile("Data/brick1.png");
        wallSprite.setTexture(wall);

        wall2.loadFromFile("Data/brick2.png");
        wall2Sprite.setTexture(wall2);

        spikes.loadFromFile("Data/spike.png");
        spikesSprite.setTexture(spikes);

        if (!font.loadFromFile("font/arial.ttf")) 
        { 
            cout << "Font didnt load\n";
        }

        // Setup lives text
        livesText.setFont(font);
        livesText.setCharacterSize(24);
        livesText.setFillColor(Color::White);
        livesText.setPosition(20, 20); // Top-left corner

        // Setup rings text
        ringsText.setFont(font);
        ringsText.setCharacterSize(24);
        ringsText.setFillColor(Color::White);
        ringsText.setPosition(20, 50); // Below lives text

        timerText.setFont(font);
        timerText.setCharacterSize(30);
        timerText.setFillColor(Color::White);
        timerText.setPosition(20, (12.5)*64);

        lvltimer->start();

        dooropen.loadFromFile("Data/dooropen.png");
        doorclosed.loadFromFile("Data/closeddoor.png");

        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(Color::White);
        scoreText.setPosition(20, 80); 

        levelIntroText.setFont(font);
        levelIntroText.setCharacterSize(72);
        levelIntroText.setFillColor(Color::Yellow);
        levelIntroText.setStyle(Text::Bold);
        levelIntroText.setString("LEVEL ?");

        levelIntroText.setPosition(500, 200); // Center of 1200x896 window

        levelIntroClock.restart();

        for (int i = 0;i < enemyCapacity;i++)
        {
            enemies[i] = nullptr;
        }
	}

    void resetLevelIntro() {
        showLevelIntro = true;
        levelIntroClock.restart();
    }

    void updateTimerDisplay() 
    {
        float remaining = lvltimer->getRemainingTime();
        int minutes = static_cast<int>(remaining) / 60;
        int seconds = static_cast<int>(remaining) % 60;
        timerText.setString(
            "TIME: " +
            to_string(minutes) + ":" +
            (seconds < 10 ? "0" : "") + to_string(seconds)
        );
    }

    void drawTimer(RenderWindow& window) 
    {
        window.draw(timerText);
    }

    bool isTimeUp() const 
    {
        return lvltimer->timeup();
    }
    void resetTimer() {
        lvltimer->setDura(lvlTime);
        lvltimer->start();
    }

    float getLevelTime()
    {
        return lvlTime;
    }
    virtual void setObstaclesandCollectibles() = 0;
    void playersInputs()
    {
        //controls for the leaders movements
        if (Keyboard::isKeyPressed(Keyboard::Right))
            manager.getLeader()->moveRight(lvl,64);
        else if (Keyboard::isKeyPressed(Keyboard::Left))
            manager.getLeader()->moveLeft(lvl,64);
        else
            manager.getLeader()->setVelocityX(0);

        if (Keyboard::isKeyPressed(Keyboard::Space))
            manager.getLeader()->jump();
    }

    void checkCollectibles(Player* player, CollectibleFactory& factory, char** lvl, int cell_size) {
        for (int i = 0; i < factory.getCount();) 
        {
            Collectibles* c = factory.getCollectibles()[i];
            
            if (!c) //since some are null pointers 
            {
                i++; 
                continue;
            }

            // hitbox
            float px = player->getXposition();
            float py = player->getYPosition();
            float cx = c->getX(cell_size);
            float cy = c->getY(cell_size);

            //collision from all 4 sides
            if (px + player->HITBOX_RIGHT > cx && px + player->HITBOX_LEFT < cx + cell_size && py + player->HITBOX_BOTTOM > cy && py + player->HITBOX_TOP < cy + cell_size) 
            {
                if (c->typeOfCollectible() == "ring")
                    ringscollected += 1;
                else if (c->typeOfCollectible() == "extralife")
                    manager.addLife();
                else
                    manager.getLeader()->useSpecialAbility();

                c->collect(lvl); 
                factory.removeCollectible(i);
            }
            else 
            {
                i++; //if we didnt remove smth
            }
        }
    }
    int getScore()
    {
        return score;
    }

    virtual void trigger(RenderWindow& window, float offset) = 0;

    void drawCollectibles(RenderWindow& window, float offset)
    {
        for (int i = 0; i < factory.getCount(); ++i) {
            factory.getCollectibles()[i]->draw(64, window, offset);
        }
    }

    void display_level(RenderWindow& window, const int height, const int width, char** lvl, Sprite& wallSprite1, const int cell_size, float offset)
    {
        for (int i = 0; i < height; i += 1)
        {
            for (int j = 0; j < width; j += 1)
            {
                if (lvl[i][j] == 'w')
                {
                    wallSprite1.setPosition(j * cell_size - offset, i * cell_size);
                    window.draw(wallSprite1);
                }
                if (lvl[i][j] == 'v') //represents breakable wall
                {
                    wall2Sprite.setPosition(j * cell_size - offset, i * cell_size);
                    window.draw(wall2Sprite);
                }
                if (lvl[i][j] == 's')
                {
                    spikesSprite.setPosition(j * cell_size - offset, i * cell_size);
                    window.draw(spikesSprite);
                }
            }
        }
    }

    void collisionsWithSpikes(Player* player)
    {
        //players boundaries
        float px_l = player->getXposition() + player->HITBOX_LEFT;
        float px_r = player->getXposition() + player->HITBOX_RIGHT;
        float py_t = player->getYPosition() + player->HITBOX_TOP;
        float py_b = player->getYPosition() + player->HITBOX_BOTTOM;
        for (int i = 0; i < height; i += 1)
        {
            for (int j = 0; j < width; j += 1)
            {
                if (lvl[i][j] == 's')
                {
                    float sx_l = j * cell_size;
                    float sy_t = i * cell_size;
                    float sx_r = (j+1) * cell_size;
                    float sy_b = (i+1) * cell_size;

                    if (!player->isInvincible()&& px_r > sx_l&& px_l < sx_r&& py_b> sy_t&& py_t < sy_b )
                    {
                        manager.removeLife();
                        player->setVelocityY(-10.f);
                        return;
                    }

                }
            }
        }
    }
    void physics()
    {
        //physics
        for (int i = 0; i < 3; ++i)
        {
            manager.getPlayer(i)->updatePhysics();
            manager.getPlayer(i)->updatePhysicsWithCollision(lvl, cell_size);
            
        }
        checkCollectibles(manager.getLeader(), factory, lvl, cell_size); //only the leader collects collectibles or collides with the spikes
        collisionsWithSpikes(manager.getLeader());
        manager.checkPits();
        int enemiesKilled=Enemy::checkcollision(manager, enemies, enemyCount);
        score += enemiesKilled * 50; //killing one enemy gives 50 points
        //followers
        manager.updateFollowers();
    }
    void addEnemy(int type, float x, float y) {
        if (enemyCount >= enemyCapacity) {
            // Resize array
            enemyCapacity *= 2;
            Enemy** newEnemies = new Enemy * [enemyCapacity];
            for (int i = 0; i < enemyCount; ++i)
                newEnemies[i] = enemies[i];
            delete[] enemies;
            enemies = newEnemies;
        }

        enemies[enemyCount++] = EnemyFactory::createEnemy(type, x, y, width, height);
    }

    void updateEnemies(Player* player, char** lvl, float cellSize) {
        for (int i = 0; i < enemyCount; ++i) {
            if (enemies[i] && enemies[i]->isAlive())
                enemies[i]->updateposi(player, lvl, cellSize);
        }
    }
    void drawEnemies(RenderWindow& window, float scrollOffsetx) {
        for (int i = 0; i < enemyCount; ++i) {
            if (enemies[i] && enemies[i]->isAlive())
                enemies[i]->draw(window,scrollOffsetx);
        }
    }

    void render(RenderWindow& window, float scrollOffsetX) {
        lvlSprite.setPosition(-scrollOffsetX, 0);
        window.draw(lvlSprite);
        trigger(window, scrollOffsetX);


        for (int i = 0; i < 3; ++i) {
            Sprite s = manager.getPlayer(i)->getSprite();
            float x = manager.getPlayer(i)->getXposition() - scrollOffsetX;
            float y = manager.getPlayer(i)->getYPosition();

            s.setPosition(x, y);
            window.draw(s);
        }
        drawCollectibles(window, scrollOffsetX);
        display_level(window, height, width, lvl, wallSprite, 64, scrollOffsetX);
        updateTimerDisplay();

        livesText.setString("Lives: " + to_string(manager.getLives()));
        ringsText.setString("Rings: " + to_string(ringscollected));
        scoreText.setString("Score: " + to_string(score));


        window.draw(livesText);
        window.draw(ringsText);
        window.draw(scoreText);
        drawEnemies(window, scrollOffsetX);
        drawTimer(window);

        if (showLevelIntro) 
        {
            window.draw(levelIntroText);

            if (levelIntroClock.getElapsedTime().asSeconds() >= 2.0f) {
                showLevelIntro = false;
            }
        }
    }
    bool run(RenderWindow& window) {
        float scrollOffsetX = 0.0f;
        Event ev;

        if (manager.isGameOver())
            window.close();

        while (window.isOpen())
        {
            while (window.pollEvent(ev))
            {
                if (ev.type == Event::Closed)
                    window.close();
                if (ev.type == Event::KeyPressed)
                {
                    if (ev.key.code == Keyboard::Z)
                        manager.changePlayer();
                }
            }

            if (manager.isGameOver() || isTimeUp()) {
                return false; // Level failed
            }

            if (lvlFinished) {
                return true;  //successfully completed the level
            }

            playersInputs();
            physics();
            updateEnemies(manager.getLeader(), lvl, cell_size);

            // scrolling
            float leaderX = manager.getLeader()->getXposition();
            scrollOffsetX = leaderX - 640;
            if (scrollOffsetX < 0)  //meaning when we havent scrolled at all
                scrollOffsetX = 0;
            float maxScrollX = (width * cell_size) - 1280; //1280 = window width
            if (scrollOffsetX > maxScrollX)
                scrollOffsetX = maxScrollX;




            //render
            window.clear(Color::Black);
            render(window, scrollOffsetX);
            window.display();
        }
        return false;
    }
    virtual ~Levels() 
    {
        for (int i = 0; i < height; i++) 
        {
            delete[] lvl[i];
        }
        delete[] lvl;

        for (int i = 0; i < enemyCount; ++i) 
        {
            delete enemies[i];
        }
        delete[] enemies;
    }
    bool isLvlOver()
    {
        return lvlFinished;
    }
};

class Level1 :public Levels {
public:
    Level1(Timer& timer):Levels(14,200,5,1,0.8,1,120.0f,timer)
    { 
        levelIntroText.setString("LEVEL 1");

        lvlSprite.setTexture(lvlTex1);
        lvlSprite.setScale(2.f, 3.f);
        lvlSprite.setTextureRect(IntRect(0, 0, width * 64, height * 64));
        setObstaclesandCollectibles();
       
        //to stretch the background to the size of the window required
    }
    virtual void setObstaclesandCollectibles() override
    {

        //basic level layout
        for (int i = 0;i < 200;i++) //originally setting wall on the whole ground
        {
            lvl[12][i] = 'w';
            lvl[13][i] = 'w';
        }
        lvl[0][0] = 'w';
        lvl[1][0] = 'w';

        for (int i = 3;i < 200;i++) //originally setting wall on the whole ground
        {
            lvl[0][i] = 'w';
            lvl[1][i] = 'w';
        }

        for (int i = 0;i < 200;i++) //originally setting wall on the whole ground
        {
            lvl[12][i] = 'w';
            lvl[13][i] = 'w';
        }

        for (int row = 10; row <= 12; row++) {
            lvl[row][5] = 'w'; // Left wall
            lvl[row][10] = 'w'; // Right wall
        }
        addEnemy(0, 11 * cell_size, 11 * cell_size);
        addEnemy(0, 60 * cell_size, 11 * cell_size);
        addEnemy(1, 7 * cell_size, 11 * cell_size);
        addEnemy(1, 40 * cell_size, 11 * cell_size);
        addEnemy(2, 4 * cell_size,  3* cell_size);
            addEnemy(3, 10 * cell_size, 5* cell_size);
        factory.spawn(new Rings(10, 8, lvl));
        rings++;
        for (int i = 10;i < 14;i++)
        {
            lvl[10][i] = 'w';
        }
        for (int i = 14;i < 22;i++)
        {
            lvl[8][i] = 'w';
        }
        for (int i = 22;i < 42;i%2==0?i+=1:i+=2)
        {
            lvl[6][i] = 'w';
        }
        for (int i = 44;i < 54;i+=1)
        {
            lvl[5][i] = 'w';
        }
        for (int i = 54;i < 64;i += 2)
        {
            lvl[5][i] = 'w';
        }
        factory.spawn(new SpecialAbility(11, 22, lvl));

        lvl[11][24] = 'v';
        lvl[11][30] = 'v';
        lvl[11][38] = 'v';


        for (int i = 48;i < 50;i += 1)
        {
            lvl[12][i] = 'p';
            lvl[13][i] = 'p';
        }
        lvl[11][15] = 'w'; // Wall
        addEnemy(0, 12 * 64, 11 * 64);
        for (int i = 10;i < 30;i++)
        {
            lvl[9][i] = 'w';
        }
        for (int i = 30;i < 35;i+=2)
        {
            lvl[7][i] = 'w';
            factory.spawn(new Rings(5, i, lvl));
            rings++;
        }
        for (int i = 35;i < 45;i++)
        {
            lvl[5][i] = 'w';
        }
        factory.spawn(new ExtraLives(3, 40, lvl));

        
        //tails special region so it can fly  over it
        factory.spawn(new SpecialAbility(11, 48, lvl));
        for (int i = 50;i < 75;i++)
        {
            lvl[12][i] = 's';
        }
        
        //sonic special region so it can run over it very fast
        factory.spawn(new SpecialAbility(11, 98, lvl));
        for (int i = 100;i < 140;i += 4)
        {
            factory.spawn(new Rings(11, i, lvl));
            rings++;
        }
        for (int i = 100;i < 125;i++)
        {
            lvl[5][i] = 'w';
        }
        factory.spawn(new ExtraLives(10, 115, lvl));


        for (int i = 10;i < 12;i++)
            lvl[12][i] = 'p';

    } 
    virtual void trigger(RenderWindow& window, float offset = 0) override
    {
        door.setScale(2.5f, 2.5f);
        if (manager.getLeader()->getXposition() >= 190 * 64.f && manager.getLeader()->getXposition() <= 193 * 64.f && manager.getLeader()->getYPosition() <= 12 * 64 && manager.getLeader()->getYPosition() >= 10 * 64 && ringscollected == rings)
        {
            door.setPosition(191 * 64-offset, 8.7 * 64);
            door.setTexture(dooropen);
            window.draw(door);
            lvlFinished = true;
        }
        else
        {
            door.setPosition(191 * 64-offset, 8.7 * 64);
            door.setTexture(doorclosed);
            window.draw(door);
        }
    }
};

class Level2 :public Levels {
public:
    Level2(Timer& timer) :Levels(14, 250, 20, 1, 1, 1,90.0f, timer)
    {
        levelIntroText.setString("LEVEL 2");

        setObstaclesandCollectibles();
        lvlSprite.setTexture(lvlTex2);
        lvlSprite.setScale(2.f, 3.f);
        lvlSprite.setTextureRect(IntRect(0, 0, width * 64, height * 64));
    }
    virtual void setObstaclesandCollectibles() override
    {
        //basic level layout
        for (int i = 0;i < width;i++) //originally setting wall on the whole ground
        {
            lvl[12][i] = 'w';
            lvl[13][i] = 'w';
        }
        lvl[0][0] = 'w';
        lvl[1][0] = 'w';

        for (int i = 3;i < width;i++) //originally setting wall on the whole ground
        {
            lvl[0][i] = 'w';
            lvl[1][i] = 'w';
        }


        factory.spawn(new Rings(10, 8, lvl));
        rings++;
        for (int i = 10;i < 14;i++)
        {
            lvl[10][i] = 'w';
        }
        for (int i = 14;i < 22;i++)
        {
            lvl[8][i] = 'w';
        }
        for (int i = 22;i < 42;i % 2 == 0 ? i += 1 : i += 2)
        {
            lvl[6][i] = 'w';
        }
        for (int i = 44;i < 54;i += 1)
        {
            lvl[5][i] = 'w';
        }
        for (int i = 54;i < 64;i += 2)
        {
            lvl[5][i] = 'w';
        }
        factory.spawn(new SpecialAbility(11, 22, lvl));

        lvl[11][24] = 'v';
        lvl[11][30] = 'v';
        lvl[11][38] = 'v';


        for (int i = 48;i < 50;i += 1)
        {
            lvl[12][i] = 'p';
            lvl[13][i] = 'p';
        }

    }
    virtual void trigger(RenderWindow& window, float offset = 0) override
    {
        door.setScale(2.5f, 2.5f);
        if (manager.getLeader()->getXposition() >= 240 * 64.f && manager.getLeader()->getXposition() <= 243 * 64.f && manager.getLeader()->getYPosition() <= 12 * 64&& manager.getLeader()->getYPosition() >= 10 * 64 && ringscollected == rings)
        {
            door.setPosition(241 * 64 - offset, 8.7 * 64);
            door.setTexture(dooropen);
            window.draw(door);
            sleep(seconds(2));
            lvlFinished = true;
        }
        else
        {
            door.setPosition(241 * 64-offset, 8.7 * 64);
            door.setTexture(doorclosed);
            window.draw(door);
        }
    }
};


class Level3 :public Levels {
public:
    Level3(Timer& timer) :Levels(14, 300, 25, 1, 0.3, 0.5,120.0f, timer)
    {
        levelIntroText.setString("LEVEL 3");

        setObstaclesandCollectibles();

        lvlSprite.setTexture(lvlTex3);
        lvlSprite.setScale(1, 1.2f);
        lvlSprite.setTextureRect(IntRect(0, 0, width * 64, height * 64));
    }
    virtual void setObstaclesandCollectibles() override
    {
        //basic level layout
        for (int i = 0;i < width;i++) //originally setting wall on the whole ground
        {
            lvl[12][i] = 'w';
            lvl[13][i] = 'w';
        }
        lvl[0][0] = 'w';
        lvl[1][0] = 'w';

        for (int i = 3;i < width;i++) //originally setting wall on the whole ground
        {
            lvl[0][i] = 'w';
            lvl[1][i] = 'w';
        }

        for (int i = 0;i < 200;i++) //originally setting wall on the whole ground
        {
            lvl[12][i] = 'w';
            lvl[13][i] = 'w';
        }

        factory.spawn(new Rings(10, 8, lvl));
        rings++;
        for (int i = 10;i < 14;i++)
        {
            lvl[10][i] = 'w';
        }
        for (int i = 14;i < 22;i++)
        {
            lvl[8][i] = 'w';
        }
        for (int i = 22;i < 42;i % 2 == 0 ? i += 1 : i += 2)
        {
            lvl[6][i] = 'w';
        }
        for (int i = 44;i < 54;i += 1)
        {
            lvl[5][i] = 'w';
        }
        for (int i = 54;i < 64;i += 2)
        {
            lvl[5][i] = 'w';
        }
        factory.spawn(new SpecialAbility(11, 22, lvl));

        lvl[11][24] = 'v';
        lvl[11][30] = 'v';
        lvl[11][38] = 'v';


        for (int i = 48;i < 50;i += 1)
        {
            lvl[12][i] = 'p';
            lvl[13][i] = 'p';
        }



    }
    virtual void trigger(RenderWindow& window, float offset = 0) override
    {
        door.setScale(2.5f, 2.5f);
        if (manager.getLeader()->getXposition() >= 290 * 64.f && manager.getLeader()->getXposition() <= 293 * 64.f && manager.getLeader()->getYPosition() <= 12 * 64 && manager.getLeader()->getYPosition() >= 10 * 64 && ringscollected == rings)
        {
            door.setPosition(291 * 64 - offset, 8.7 * 64);
            door.setTexture(dooropen);
            window.draw(door);
            sleep(seconds(2));
            lvlFinished = true;
        }
        else
        {
            door.setPosition(291 * 64-offset, 8.7 * 64);
            door.setTexture(doorclosed);
            window.draw(door);
        }
    }
};
class BossLevel :public Levels {
    public:
    BossLevel(Timer& timer) :Levels(14, 32, 25, 1, 1.2, 1.5, 120.0f, timer)
    {
        levelIntroText.setString("BOSS LEVEL");

        setObstaclesandCollectibles();
        lvlSprite.setTexture(bossleveltex);
        lvlSprite.setScale(1.0f, 1.0f);
        lvlSprite.setTextureRect(IntRect(0, 0, width * 64, height * 64));
    }
    virtual void setObstaclesandCollectibles() override
    {
        //basic level layout
        for (int i = 0;i < width;i++) //originally setting wall on the whole ground
        {
            lvl[12][i] = 'w';
            lvl[13][i] = 'w';
        }
        lvl[0][0] = 'w';
        lvl[1][0] = 'w';

        for (int i = 3;i < width;i++) //originally setting wall on the whole ground
        {
            lvl[0][i] = 'w';
            lvl[1][i] = 'w';
        }

        for (int i = 0;i < 64;i++) //originally setting wall on the whole ground
        {
            lvl[12][i] = 'w';
            lvl[13][i] = 'w';
        }

        factory.spawn(new Rings(12, 9, lvl));
        rings++;
        for (int i = 10;i < 14;i++)
        {
            lvl[8][i] = 'w';
        }
        for (int i = 14;i < 22;i++)
        {
            lvl[12][i] = 'w';
        }
        for (int i = 22;i < 42;i % 2 == 0 ? i += 1 : i += 2)
        {
            lvl[2][i] = 'w';
        }
        for (int i = 44;i < 54;i += 1)
        {
            lvl[5][i] = 'w';
        }
        for (int i = 54;i < 64;i += 2)
        {
            lvl[5][i] = 'w';
        }
        factory.spawn(new SpecialAbility(11, 22, lvl));

        lvl[11][24] = 'v';
        lvl[11][30] = 'v';
        lvl[11][38] = 'v';

        addEnemy(4, 4 * cell_size, 3 * cell_size);
        for (int i = 48;i < 50;i += 1)
        {
            lvl[12][i] = 'p';
            lvl[13][i] = 'p';
        }



    }
    virtual void trigger(RenderWindow& window, float offset = 0) override
    {
        door.setScale(2.5f, 2.5f);
        if (manager.getLeader()->getXposition() >= 60 * 64.f && manager.getLeader()->getXposition() <= 63 * 64.f && manager.getLeader()->getYPosition() <= 12 * 64 && manager.getLeader()->getYPosition() >= 10 * 64 && ringscollected == rings)
        {
            door.setPosition(62* 64 - offset, 8.7 * 64);
            door.setTexture(dooropen);
            window.draw(door);
            sleep(seconds(2));
            lvlFinished = true;
        }
        else
        {
            door.setPosition(62 * 64 - offset, 8.7 * 64);
            door.setTexture(doorclosed);
            window.draw(door);
        }
    }

};

class LevelManager {
    const int MAXLEVELS = 4;
    Levels** levels = new Levels * [MAXLEVELS];
    int currentLevel;
    Timer timer;
    int scores[4]{0};

    void loadLevels() 
    {
        levels[0] = new BossLevel(timer);
        levels[1] = new Level2(timer); 
        levels[2] = new Level3(timer); 
        levels[3] = new BossLevel(timer);
    }
public:
    LevelManager() : currentLevel(0) ,timer(120.f)
    {
        loadLevels(); 
    }
    ~LevelManager() {
        for (int i = 0; i < MAXLEVELS; ++i) 
        {
            if (levels[i]) 
            {
                delete levels[i];
                levels[i] = nullptr;
            }
        }
    }
    bool runCurrentLevel(RenderWindow& window) 
    {
        if (currentLevel >= MAXLEVELS || !levels[currentLevel]) 
        {
            return false;
        }

        bool levelCompleted = levels[currentLevel]->run(window);
        scores[currentLevel] = levels[currentLevel]->getScore();

        if (levelCompleted) 
        {
            sleep(seconds(2)); // Brief pause to see completion
            return true;
        }
        else {
            // Game over occurred
            showGameIsOver(window);
            return false;
        }
        return false;

    }
    void showGameIsOver(RenderWindow& window) 
    {
        Font font;
        if (!font.loadFromFile("font/arial.ttf")) 
        {
            return;
        }

        Text gameOverText;
        gameOverText.setFont(font);
        gameOverText.setString("GAME OVER");
        gameOverText.setCharacterSize(72);
        gameOverText.setFillColor(Color::Red);
        gameOverText.setStyle(Text::Bold);


        gameOverText.setPosition(400, 300);

        Clock displayClock;
        while (window.isOpen() && displayClock.getElapsedTime().asSeconds() < 5.0f) {
            Event ev;
            while (window.pollEvent(ev)) 
            {
                if (ev.type == Event::Closed) {
                    window.close();
                }
                if (ev.type == Event::KeyPressed) 
                {
                    return;
                }
            }

            window.clear(Color::Black);
            window.draw(gameOverText);
            window.display();
        }
    }
    void nextLevel() 
    {
        if (currentLevel < MAXLEVELS - 1) 
        {
            currentLevel++;
            levels[currentLevel]->resetLevelIntro();
            timer.setDura(levels[currentLevel]->getLevelTime()); // Add this method to Levels
            timer.start();
        }
    }
    bool gameCompleted() const 
    {
        return (currentLevel >= MAXLEVELS - 1);
    }
    void reset() 
    {
        currentLevel = 0;
    }
    int* getscore()
    {
        return scores;
    }
};


