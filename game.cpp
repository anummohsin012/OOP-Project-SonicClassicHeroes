#include <SFML/Graphics.hpp>
#include "menu.cpp"
#include "levels.cpp"

using namespace sf;
using namespace std;

class ScoreBoard {
    int scores[4]{ 0 };
    int total_score;
    string name;
public:
    ScoreBoard() :total_score(0), name("") {}
    void setname(string name) { this->name = name; }
    void setscores(int sc[])
    {
        for (int i = 0;i < 4;i++)
        {
            scores[i] = sc[i];
            total_score += scores[i];
        }

    }
    void display()
    { }
};

class Game {
private:
    RenderWindow window;
    Menu menu;
    LevelManager levelManager;
    int gameState;
    string playerName;
    ScoreBoard scoreBoard;

public:
    Game() : window(VideoMode(1200, 896), "Sonic Classic Heroes"),menu(1200, 896),levelManager(), gameState(0), playerName("Player")
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
                    gameState = 0;
                    levelManager.reset();
                }
                else 
                {
                    levelManager.nextLevel();
                }
            }
            else
            {
                gameState = 0;
                levelManager.reset();
            }
        }
    }

    void render() {
        window.clear();

        if (gameState == 0) {
            menu.draw(window, 900, 1200);
        }

        window.display();
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}

