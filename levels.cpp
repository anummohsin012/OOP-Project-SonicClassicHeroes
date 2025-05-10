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
	Texture lvlTex;
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
   /* Enemy** enemies;
    int enemyCapacity;
    int enemyCount;*/

public:
	Levels(int h, int w, int rings, int e_l, float a, float g,float duration, Timer& timer)
		:height(h), width(w), cell_size(64), lvl(new char* [height]), manager(playerfactory,lvl, height, width,a, g), rings(rings), ringscollected(0), score(0), exlives(e_l), factory(rings+e_l), lvlTime(duration),lvltimer(&timer)/*enemyCount(0),enemyCapacity(1)*/
	{
        for (int i = 0; i < height; i ++) 
        {
            lvl[i] = new char[width];
            for (int j = 0; j < width; j ++) 
            {
                lvl[i][j] = ' '; // Initialize all cells to empty
            }
        }
        if (!lvlTex.loadFromFile("Data/bglvl1.png"))
        {
            return;
        }

        lvlSprite.setTexture(lvlTex);
        lvlSprite.setScale((float)(width * cell_size) / lvlTex.getSize().x, 3.0f); //to stretch the background to the size of the window required

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
        doorclosed.loadFromFile("Data/doorclosed.png");
	}
    void updateTimerDisplay() {
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

        //followers
        manager.updateFollowers();
        //manager.checkPitRespawns();
    }
    //void addEnemy(int type, float x, float y) {
    //    if (enemyCount >= enemyCapacity) {
    //        // Resize array
    //        enemyCapacity *= 2;
    //        Enemy** newEnemies = new Enemy * [enemyCapacity];
    //        for (int i = 0; i < enemyCount; ++i)
    //            newEnemies[i] = enemies[i];
    //        delete[] enemies;
    //        enemies = newEnemies;
    //    }

    //    enemies[enemyCount++] = EnemyFactory::createEnemy(type, x, y, width, height);
    //}

    //void updateEnemies(Player* player, char** lvl, float cellSize) {
    //    for (int i = 0; i < enemyCount; ++i) {
    //        if (enemies[i] && enemies[i]->isAlive())
    //            enemies[i]->updateposi(player, lvl, cellSize);
    //    }
    //}
    //void drawEnemies(RenderWindow& window) {
    //    for (int i = 0; i < enemyCount; ++i) {
    //        if (enemies[i] && enemies[i]->isAlive())
    //            enemies[i]->draw(window);
    //    }
    //}

    void render(RenderWindow& window, float scrollOffsetX) {
        lvlSprite.setPosition(-scrollOffsetX, 0);
        window.draw(lvlSprite);

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

        window.draw(livesText);
        window.draw(ringsText);
        /*drawEnemies(window);*/
        drawTimer(window);
    }
    void run(RenderWindow& window) {
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

            playersInputs();
            physics();

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
    }
    virtual ~Levels() 
    {
        for (int i = 0; i < height; i++) 
        {
            delete[] lvl[i];
        }
        delete[] lvl;
    }

};

class Level1 :public Levels {
public:
    Level1(Timer& timer):Levels(14,200,5,1,0.8,1,120.0f,timer)
    { 
        setObstaclesandCollectibles();
    }
    virtual void setObstaclesandCollectibles() override
    {

        /*addEnemy(1, 150.0f, 200.0f); 
        addEnemy(2, 300.0f, 150.0f);*/
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

        factory.spawn(new Rings(10, 8, lvl));
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


        //for (int i = 10;i < 30;i++)
        //{
        //    lvl[9][i] = 'w';
        //}
        //for (int i = 30;i < 35;i+=2)
        //{
        //    lvl[7][i] = 'w';
        //    factory.spawn(new Rings(5, i, lvl));
        //}
        //for (int i = 35;i < 45;i++)
        //{
        //    lvl[5][i] = 'w';
        //}
        //factory.spawn(new ExtraLives(3, 40, lvl));

        //
        ////tails special region so it can fly  over it
        //factory.spawn(new SpecialAbility(11, 48, lvl));
        //for (int i = 50;i < 75;i++)
        //{
        //    lvl[12][i] = 's';
        //}
        //
        ////sonic special region so it can run over it very fast
        //factory.spawn(new SpecialAbility(11, 98, lvl));
        //for (int i = 100;i < 140;i += 4)
        //{
        //    factory.spawn(new Rings(12, i, lvl));
        //}
        //for (int i = 100;i < 125;i++)
        //{
        //    lvl[5][i] = 'w';
        //}
        //factory.spawn(new ExtraLives(10, 115, lvl));


        //for (int i = 10;i < 12;i++)
        //    lvl[12][i] = 'p';

    } 
};

class Level2 :public Levels {
public:
    Level2(Timer& timer) :Levels(14, 250, 20, 1, 1, 1,90.0f, timer)
    {
        setObstaclesandCollectibles();
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

        factory.spawn(new Rings(10, 8, lvl));
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
};


class Level3 :public Levels {
public:
    Level3(Timer& timer) :Levels(14, 300, 25, 1, 0.3, 0.5,120.0f, timer)
    {
        setObstaclesandCollectibles();
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

        factory.spawn(new Rings(10, 8, lvl));
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
};

int main()
{
    RenderWindow window(VideoMode(1200, 896), "Sonic Classic Heroes");
    Timer timer(120.0f);
    window.setFramerateLimit(60);
    Level1 level1(timer);
    Level2 lvl2(timer);

    level1.run(window); // Launch full game loop from Level1
    lvl2.run(window);

    return 0;
}


