#include <SFML/Graphics.hpp>
#include "menu.cpp"
#include "levels.cpp"

using namespace sf;
using namespace std;

class ScoreBoard {
    RenderWindow& window;
    int scores[4]{ 0 };
    int total_score;
    string name;
    Texture tex;
    Sprite spr;
    Font font;
    Text texts[6];

public:
    ScoreBoard(RenderWindow& win) :total_score(0), name(""), window(win) {
        tex.loadFromFile("Data/scoreboard.png");
        spr.setTexture(tex);
        float scaleX = 1200.f / 300.f;
        float scaleY = 896.f / 168.f;
        spr.setScale(scaleX, scaleY);
        if (!font.loadFromFile("font/arial.ttf")) {
            cout << "Failed to load font\n";
        }

        for (int i = 0; i < 6; i++) 
        {
            texts[i].setFont(font);
            texts[i].setCharacterSize(70);
            texts[i].setFillColor(Color::Red);
            texts[i].setStyle(Text::Bold);
        }
    }
    void setname(string name) { this->name = name; 
    updateTexts();
    }
    void setscores(int sc[])
    {
        for (int i = 0;i < 4;i++)
        {
            scores[i] = sc[i];
            total_score += scores[i];
        }
        updateTexts();
    }
    void updateTexts() {
        texts[0].setString("Player: " + name);
        texts[1].setString("Level 1 Score: " + to_string(scores[0]));
        texts[2].setString("Level 2 Score: " + to_string(scores[1]));
        texts[3].setString("Level 3 Score: " + to_string(scores[2]));
        texts[4].setString("Level 4 Score: " + to_string(scores[3]));
        texts[5].setString("Total Score: " + to_string(total_score));

        for (int i = 0; i < 6; i++) 
        {
            texts[i].setPosition(330, 250 + i * 70); // Adjust X, Y positions
        }
    }
    void display()
    { 
        window.draw(spr);
        for (int i = 0; i < 6; i++) {
            window.draw(texts[i]);
        }
    }
};

class Game {
private:
    RenderWindow window;
    Menu menu;
    LevelManager levelManager;
    int gameState; //0 is menu 1 is game 3 is scoreboard
    string playerName;
    ScoreBoard scoreBoard;

public:
    Game() : window(VideoMode(1200, 896), "Sonic Classic Heroes"),menu(1200, 896),levelManager(), gameState(0), playerName("Player"),scoreBoard(window)
    {}

    void run() {
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }

private:
    void handleEvents() {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::KeyPressed) {
                if (gameState == 0) {
                    if (!menu.getoptionopen()) {
                        if (event.key.code == Keyboard::Up)
                            menu.hower(-1);
                        else if (event.key.code == Keyboard::Down)
                            menu.hower(1);
                        else if (event.key.code == Keyboard::Enter) {
                            int selected = menu.getindex();
                            if (selected == 0) {
                                menu.nameinput(window, 896, 1200);
                                if (menu.getcanstart()) {
                                    playerName = menu.getPlayerName();
                                    scoreBoard.setname(playerName);
                                    gameState = 1;
                                }
                            }
                            else if (selected == 4) {
                                window.close();
                            }
                            else {
                                menu.optionselected(selected, window, 900, 1200);
                            }
                        }
                    }
                    else {
                        if (event.key.code == Keyboard::Up)
                            menu.volset(1);
                        else if (event.key.code == Keyboard::Down)
                            menu.volset(0);
                        else if (event.key.code == Keyboard::Escape)
                            menu.toggleing();
                    }
                }
                else if (gameState == 1) 
                {
                    if (event.key.code == Keyboard::Escape) {
                        gameState = 0;
                    }
                    
                }
                else if (gameState == 2)
                {
                    if (event.key.code == Keyboard::Escape) {
                        gameState = 0;
                    }
                }

            }
        }
    }

    void update() {
        window.setFramerateLimit(60);
        if (gameState == 1) {
            
            bool levelCompleted = levelManager.runCurrentLevel(window);

            if (levelCompleted) 
            {
                if (levelManager.gameCompleted()) 
                {
                    settingScoreBoard();
                    gameState = 2;
                    levelManager.reset();
                }
                else 
                {
                    levelManager.nextLevel();
                }
            }
            else
            {
                settingScoreBoard();
                gameState = 2;
                levelManager.reset();
            }
        }
    }

    void settingScoreBoard()
    {
        scoreBoard.setname(playerName);
        scoreBoard.setscores(levelManager.getscore());
    }

    void render() {
        window.clear();

        if (gameState == 0) {
            menu.draw(window, 900, 1200);
        }
        else if (gameState == 2) {
            scoreBoard.display();
        }

        window.display();
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}

