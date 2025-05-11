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
                lvl[i][j] = ' '; // all cells empty
            }
        }

        //lvl backgrounds
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
        

        //wall
        wall.loadFromFile("Data/brick1.png");
        wallSprite.setTexture(wall);

        //breakable wall
        wall2.loadFromFile("Data/brick2.png");
        wall2Sprite.setTexture(wall2);

        spikes.loadFromFile("Data/spike.png");
        spikesSprite.setTexture(spikes);

        if (!font.loadFromFile("font/arial.ttf")) 
        { 
            cout << "Font didnt load\n";
        }

        // lives text
        livesText.setFont(font);
        livesText.setCharacterSize(24);
        livesText.setFillColor(Color::White);
        livesText.setPosition(20, 20); // Top-left corner

        // rings text
        ringsText.setFont(font);
        ringsText.setCharacterSize(24);
        ringsText.setFillColor(Color::White);
        ringsText.setPosition(20, 50); // Below lives text

        // timer text
        timerText.setFont(font);
        timerText.setCharacterSize(30);
        timerText.setFillColor(Color::White);
        timerText.setPosition(20, (12.5)*64);

        lvltimer->start();

        //trigger door
        dooropen.loadFromFile("Data/dooropen.png");
        doorclosed.loadFromFile("Data/closeddoor.png");

        //scores text
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(Color::White);
        scoreText.setPosition(20, 80); 

        //temporary text displaying whih level we're at
        levelIntroText.setFont(font);
        levelIntroText.setCharacterSize(72);
        levelIntroText.setFillColor(Color::Yellow);
        levelIntroText.setStyle(Text::Bold);
        levelIntroText.setString("LEVEL ?");

        levelIntroText.setPosition(500, 200); 

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
        timerText.setString("TIME: " + to_string(minutes) + ":" +(seconds < 10 ? "0" : "") + to_string(seconds) );
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

                c->collect(lvl); //removes the letter and plays the sound
                factory.removeCollectible(i); //deletes the object
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
                        player->setVelocityY(-10.f); //jumps a little
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
        score += enemiesKilled * 100; //killing one enemy gives 50 points
        //followers
        manager.updateFollowers(); //the ai following logic
    }
    void addEnemy(int type, float x, float y) {
        if (enemyCount >= enemyCapacity) {
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

    void render(RenderWindow& window, float scrollOffsetX) { //this function takes care of all the drawing functionalities
        lvlSprite.setPosition(-scrollOffsetX, 0); //scroll offset x is used to horizontally scroll based on the leaders [ositon 
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

        if (showLevelIntro)  //temporarily shows the level number then disappears
        {
            window.draw(levelIntroText);

            if (levelIntroClock.getElapsedTime().asSeconds() >= 2.0f) {
                showLevelIntro = false;
            }
        }
    }
    virtual bool run(RenderWindow& window) { //ths is the primary function that starts the level and runs it
        float scrollOffsetX = 0.0f; //starting with no scroll until leader reaches a posiotn
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
                return false; //level failed
            }

            if (lvlFinished) {
                return true;  //successfully completed the level
            }

            playersInputs();
            physics();
            updateEnemies(manager.getLeader(), lvl, cell_size);

            // scrolling
            float leaderX = manager.getLeader()->getXposition();
            scrollOffsetX = leaderX - 640; //allows the screen to scroll as leader moves
            if (scrollOffsetX < 0)  //meaning when we havent scrolled at all 
                scrollOffsetX = 0;
            float maxScrollX = (width * cell_size) - 1200; //as window is max 1200
            if (scrollOffsetX > maxScrollX)
                scrollOffsetX = maxScrollX;


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
    Level1(Timer& timer):Levels(14,200,5,1,0.8,1,90.0f,timer)
    { 
        levelIntroText.setString("LEVEL 1");

        lvlSprite.setTexture(lvlTex1);
        lvlSprite.setScale(2.f, 3.f);
        //to fit the background to the size of the window required
        lvlSprite.setTextureRect(IntRect(0, 0, width * 64, height * 64));
        setObstaclesandCollectibles();
       
       
    }
    virtual void setObstaclesandCollectibles() override
    {
        for (int i = 0; i < 200; i++)
        {
            lvl[0][i] = 'w';
            lvl[1][i] = 'w';
            lvl[12][i] = 'w';
            lvl[13][i] = 'w';
        }

        for (int i = 1; i < 3; i++)
        {
            lvl[0][i] = ' ';
            lvl[1][i] = ' ';
        }

        for (int col = 5; col < 65; col++)
        {
            lvl[11][col] = ' ';
            if (col % 5 == 0)
            {
                factory.spawn(new Rings(10, col, lvl));
                rings++;
            }
        }

        for (int col = 10; col < 60; col += 15)
        {
            for (int w = 0; w < 4; w++)
            {
                lvl[8][col + w] = 'w';
            }
            factory.spawn(new Rings(7, col, lvl));
            rings++;
        }

        factory.spawn(new SpecialAbility(10, 62, lvl));
        addEnemy(0, 30 * cell_size, 11 * cell_size);

        for (int col = 70; col < 130; col++)
        {
            lvl[11][col] = ' ';
            if (col % 10 == 0)
            {
                factory.spawn(new Rings(6, col, lvl));
                rings++;
            }
        }

    // --- PITS in accessible areas ---
    lvl[12][35] = 'p';
    lvl[12][70] = 'p';
    lvl[12][125] = 'p';
    lvl[13][35] = 'p';
    lvl[13][70] = 'p';
    lvl[13][125] = 'p';

        factory.spawn(new SpecialAbility(10, 67, lvl));
        addEnemy(1, 105 * cell_size, 11 * cell_size);

        for (int col = 135; col < 185; col++)
        {
            lvl[11][col] = ' ';
            if (col % 6 == 0)
            {
                lvl[10][col] = 'v';
            }
            if (col % 8 == 0)
            {
                factory.spawn(new Rings(9, col, lvl));
                rings++;
            }
        }

        for (int col = 140; col < 180; col += 13)
        {
            for (int w = 0; w < 5; w++)
            {
                lvl[8][col + w] = 'w';
            }
            if (col % 3 == 0)
            {
                factory.spawn(new Rings(8, col, lvl));
                rings++;
            }
        }

        factory.spawn(new SpecialAbility(10, 132, lvl));
        addEnemy(2, 165 * cell_size, 4 * cell_size);

        factory.spawn(new ExtraLives(10, 80, lvl));
        factory.spawn(new ExtraLives(10, 180, lvl));
    }


    virtual void trigger(RenderWindow& window, float offset = 0) override
    {
        door.setScale(2.5f, 2.5f);
        if (manager.getLeader()->getXposition() >= 190 * 64.f && manager.getLeader()->getXposition() <= 193 * 64.f && manager.getLeader()->getYPosition() <= 12 * 64 && manager.getLeader()->getYPosition() >= 10 * 64 && ringscollected == rings)
        {
            //if door reached, and all rings collected the door will open and allow for the next level to start
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

        for (int col = 10; col < 40; col++)
        {
            lvl[10][i] = 'w';
        }
        factory.spawn(new SpecialAbility(10, 8, lvl));
        addEnemy(1, 18 * cell_size, 11 * cell_size);
        addEnemy(1, 34 * cell_size, 11 * cell_size);

        factory.spawn(new SpecialAbility(10, 50, lvl));
        for (int col = 60; col < 90; col += 2)
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
        addEnemy(2, 75 * cell_size, 4 * cell_size);
        factory.spawn(new ExtraLives(10, 88, lvl));

        factory.spawn(new SpecialAbility(10, 100, lvl));
        for (int col = 110; col < 180; col++)
        {
            if (col % 4 == 0)
            {
                factory.spawn(new Rings(10, col, lvl));
                rings++;
            }
            lvl[11][col] = ' ';
        }
        addEnemy(0, 130 * cell_size, 11 * cell_size);
        addEnemy(3, 175 * cell_size, 4 * cell_size);
        factory.spawn(new ExtraLives(10, 160, lvl));

        lvl[12][50] = 'p';
        lvl[13][50] = 'p';
        lvl[12][100] = 'p';
        lvl[13][100] = 'p';
        lvl[12][200] = 'p';
        lvl[13][200] = 'p';


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

        factory.spawn(new SpecialAbility(10, 120, lvl));

        addEnemy(1, 50 * cell_size, 4 * cell_size);

        for (int col = 120; col < 160; col++)
        {
            if ((col / 5) % 2 == 0)
            {
                lvl[12][col] = 's';
                lvl[13][col] = 's';
            }
            else
            {
                lvl[12][col] = 'p';
                lvl[13][col] = 'p';
            }
        }

        for (int col = 140; col < 200; col += 15)
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

        addEnemy(1, 225 * cell_size, 11 * cell_size);
        addEnemy(3, 230 * cell_size, 4 * cell_size);

        for (int i = 48;i < 50;i += 1)
        {
            lvl[12][i] = 'p';
            lvl[13][i] = 'p';
        }
        //knuckles
        factory.spawn(new SpecialAbility(10, 240, lvl));



        factory.spawn(new SpecialAbility(10, 180, lvl));
        factory.spawn(new ExtraLives(10, 100, lvl));
        factory.spawn(new ExtraLives(10, 250, lvl));
        factory.spawn(new ExtraLives(10, 280, lvl));
    }
    virtual void trigger(RenderWindow& window, float offset = 0) override
    {
    }
};
class BossLevel :public Levels {
    public:
    BossLevel(Timer& timer) :Levels(14, 18, 25, 1, 1.2, 1, 120.0f, timer)
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

        for (int i = 3;i < width;i++) 
        {
            lvl[0][i] = 'w';
            lvl[1][i] = 'w';
        }

        addEnemy(4, 4 * cell_size, 3 * cell_size);


    }
    bool run(RenderWindow& window) override 
    {
        float scrollOffsetX = 0.0f;
        Event ev;

        if (manager.isGameOver())
            window.close();

        while (window.isOpen()) {
            while (window.pollEvent(ev)) {
                if (ev.type == Event::Closed)
                    window.close();
                if (ev.type == Event::KeyPressed) {
                    if (ev.key.code == Keyboard::Z)
                        manager.changePlayer();
                }
            }

            if (manager.isGameOver() || isTimeUp()) {
                return false;
            }

            if (lvlFinished) {
                return true;
            }

            playersInputs();
            physics();
            updateEnemies(manager.getLeader(), lvl, cell_size);
            window.clear(Color::Black);
            render(window, 0.0);
            window.display();
        }
        return false;
    }
    virtual void trigger(RenderWindow& window, float offset = 0) override
    {
        door.setScale(2.5f, 2.5f);
        if (manager.getLeader()->getXposition() >= 10 * 64.f && manager.getLeader()->getXposition() <= 13 * 64.f && manager.getLeader()->getYPosition() <= 12 * 64 && manager.getLeader()->getYPosition() >= 10 * 64 && score == 50)
        {
            door.setPosition(11* 64 - offset, 8.7 * 64);
            door.setTexture(dooropen);
            window.draw(door);
            sleep(seconds(2));
            lvlFinished = true;
        }
        else
        {
            door.setPosition(11 * 64 - offset, 8.7 * 64);
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
        levels[0] = new Level1(timer);
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
        delete levels;
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
            sleep(seconds(2)); //pause
            return true;
        }
        else {
            //game lost
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
            timer.setDura(levels[currentLevel]->getLevelTime()); 
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



