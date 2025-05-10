#include <SFML/Graphics.hpp>
#include "menu.cpp"
using namespace sf;

class Game {
private:
    RenderWindow window;
    Menu menu;
    int gameState;

public:
    Game() : window(VideoMode(1280, 720), "Sonic Classic Heroes"), menu(1200,900) {
        gameState = 0;

    }

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
                if (!menu.getoptionopen()) {
                    if (event.key.code == sf::Keyboard::Up)
                        menu.hower(-1);
                    else if (event.key.code == sf::Keyboard::Down)
                        menu.hower(1);
                    else if (event.key.code == sf::Keyboard::Enter) {
                        if (menu.getindex() == 4) {
                            window.close();
                        }
                        else {
                            menu.optionselected(menu.getindex(), window, 900, 1200);
                        }
                    }
                }
                else {
                    // Option menu is open
                    menu.draw(window, 900, 1200);
                    menu.voldisplay();
                    if (event.key.code == sf::Keyboard::Up)
                        menu.volset(1);
                    else if (event.key.code == sf::Keyboard::Down)
                        menu.volset(0);
                    else if (event.key.code == sf::Keyboard::Escape)
                        menu.toggleing();
                }
            }
        }
    }


    void update() {
        // Add logic for gameplay later
    }

    void render() {
        window.clear();

        if (gameState == 0) {
            menu.draw(window,900,1200);
        }
        else if (gameState == 1) {
            
        }
        else if (gameState == 2) {
            // options screen (not implemented)
        }

        window.display();
    }
};


int main() {
    Game game;
    game.run();
    return 0;
}
