#include "player.cpp"
#include "collectables.cpp"

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
public:
	Levels(int h, int w, int rings, int e_l)
		:height(h), width(w), cell_size(64), manager(playerfactory,lvl, height, width), lvl(new char* [height]), rings(rings), ringscollected(0), score(0), exlives(e_l), factory(rings+e_l)
	{
        for (int i = 0; i < height; i ++) 
        {
            lvl[i] = new char[width];
            for (int j = 0; j < width; j ++) 
            {
                lvl[i][j] = ' '; // Initialize all cells to empty
            }
        }
        if (!lvlTex.loadFromFile("Data/lvl_layout.png"))
        {
            return;
        }

        lvlSprite.setTexture(lvlTex);
        lvlSprite.setScale((float)(width * cell_size) / lvlTex.getSize().x, 1.0f); //to stretch the background to the size of the window required

        wall.loadFromFile("Data/brick1.png");
        wallSprite.setTexture(wall);

        spikes.loadFromFile("Data/spike.png");
        spikesSprite.setTexture(spikes);

	}
    virtual void drawCollectibles(RenderWindow& window, float scrollOffsetX) = 0;
    virtual void display_level(RenderWindow& window, const int height, const int width, char** lvl, Sprite& wallSprite1, const int cell_size,float offset) = 0;
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

    void physics()
    {
        //physics
        for (int i = 0; i < 3; ++i)
        {
            manager.getPlayer(i)->updatePhysics();
            manager.getPlayer(i)->updatePhysicsWithCollision(lvl, cell_size);
            checkCollectibles(manager.getPlayer(i), factory, lvl, cell_size);
        }

        //followers
        manager.updateFollowers();
        manager.checkPitRespawns();
    }
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
    }
    void run(RenderWindow& window) {
        float scrollOffsetX = 0.0f;
        Event ev;

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

            // Scroll logic
            float leaderX = manager.getLeader()->getXposition();
            scrollOffsetX = leaderX - 640;
            if (scrollOffsetX < 0) scrollOffsetX = 0;
            float maxScrollX = (width * cell_size) - 1280; // 1280 = window width
            if (scrollOffsetX > maxScrollX)
                scrollOffsetX = maxScrollX;

            // Render
            window.clear(Color::Black);
            render(window, scrollOffsetX);
            window.display();
        }
    }
    ~Levels() 
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
    Level1():Levels(14,200,5,1)
    {
        for (int i = 0;i < 200;i++)
        {
            lvl[12][i] = 'w';
        }
        for (int i = 14;i < 16;i++)
            lvl[12][i] = 'w';
        for (int i = 24;i < 27;i++)
            lvl[8][i] = 'w';
        for (int i = 19;i < 24;i++)
            lvl[11][i] = 'w';


        factory.spawn(new Rings(8, 14,lvl));
        factory.spawn(new Rings(10, 25, lvl));
        factory.spawn(new Rings(7, 26, lvl));
        factory.spawn(new ExtraLives(10, 2, lvl));
        factory.spawn(new SpecialAbility(10, 30, lvl));
    }
    virtual void drawCollectibles(RenderWindow& window, float offset ) override 
    {
        for (int i = 0; i < factory.getCount(); ++i) {
            factory.getCollectibles()[i]->draw(64, window,offset); // assumes draw(offset, window)
        }
    }
    virtual void display_level(RenderWindow& window, const int height, const int width, char** lvl, Sprite& wallSprite1, const int cell_size, float offset)
    {
        for (int i = 0; i < height; i += 1)
        {
            for (int j = 0; j < width; j += 1)
            {
                if (lvl[i][j] == 'w')
                {
                    wallSprite1.setPosition(j * cell_size-offset, i * cell_size);
                    window.draw(wallSprite1);
                }
                if (lvl[i][j] == 's')
                {
                    spikesSprite.setPosition(j * cell_size - offset, i * cell_size);
                    window.draw(spikesSprite);
                }
            }
        }
    }

};
int main()
{
    RenderWindow window(VideoMode(1200, 900), "Sonic Classic Heroes");
    window.setFramerateLimit(60);

    Level1 level;
    level.run(window); // Launch full game loop from Level1

    return 0;
}


