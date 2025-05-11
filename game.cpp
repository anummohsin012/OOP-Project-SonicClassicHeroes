#include <SFML/Graphics.hpp>
#include "menu.cpp"
#include "levels.cpp"
#include "leaderboard.cpp"

using namespace sf;
using namespace std;
// displays player scores and names etc
class ScoreBoard {
    RenderWindow& window;
    int scores[4]{ 0 }; // to store scores for all the levels 
    int total_score; //score of all the levels combines
    string name;
    Texture tex;
    Sprite spr;
    Font font;
    Text texts[6];

public:
    ScoreBoard(RenderWindow& win) :total_score(0), name(""), window(win) {
        tex.loadFromFile("Data/scoreboard.png");
        spr.setTexture(tex);
        float scaleX = 1200.f / 600.f;
        float scaleY = 896.f / 451.f;
        spr.setScale(scaleX, scaleY);
        if (!font.loadFromFile("font/arial.ttf")) {
            cout << "Failed to load font\n";
        }

        for (int i = 0; i < 6; i++)  {
            texts[i].setFont(font);
            texts[i].setCharacterSize(70);
            texts[i].setFillColor(Color::Black);
            texts[i].setStyle(Text::Bold);
        }
    }
    void setname(string name) { this->name = name; 
    updateTexts();
    }
    void setscores(int sc[]) {
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
        for (int i = 0; i < 6; i++) {
            texts[i].setPosition(50, 50 + i * 70); // Adjust X, Y positions
        }
    }
    void display() { 
        window.draw(spr);
        for (int i = 0; i < 6; i++) {
            window.draw(texts[i]);
        }
    }
    int getTotalScore() const {
        return total_score; 
    }
};
// a class that contrils the who game at the end by compsing and aggregating differtn classes 
class Game {
private:
    RenderWindow window;
    Menu menu;
    LevelManager levelManager; // composes level manager class to controll lvels through pointers 
    int gameState; //0 is menu 1 is game 2 is scoreboard and 3 is leaderboard
    string playerName; 
    ScoreBoard scoreBoard; //composes scoreboard  to display scores wara
    Leaderboard leaderboard; // composes leaderboard class to track high scores wara

public:
    Game() : window(VideoMode(1200, 896), "Sonic Classic Heroes"),menu(1200, 896),levelManager(), gameState(0), playerName("Player"),scoreBoard(window)
    {}

    void run() { // handles all the runn that is called in the main loop
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }

private:
    // handling all the things of classes that it composed 
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
                            else if (selected == 3) { 
                                gameState = 3; 
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
                else if (gameState == 1 || gameState == 2 || gameState == 3) {
                    if (event.key.code == Keyboard::Escape) {
                        gameState = 0; 
                    }
                    else if (gameState == 2 && event.key.code == Keyboard::Enter) {
                        gameState = 0;
                    }
                }
            }
        }
    }
    //updates the states of game 
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
    //updating scoreboard and leaderboard with the help of result of scoreboard by using its getters and setters
    void settingScoreBoard()
    {
        scoreBoard.setname(playerName);
        scoreBoard.setscores(levelManager.getscore());
        leaderboard.addScore(playerName, scoreBoard.getTotalScore());
    }

    void render() {
        window.clear();

        if (gameState == 0) {
            menu.draw(window, 900, 1200);
        }
        else if (gameState == 2) {
            scoreBoard.display();
        }
        else if (gameState == 3) {
            leaderboard.draw(window);
        }

        window.display();
    }
};
// main gamee
int main() {
    Game game;
    game.run();
    return 0;
}

