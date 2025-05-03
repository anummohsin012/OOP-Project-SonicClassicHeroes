#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

using namespace std;
using namespace sf;

class Menu {
private:
	Font font;
	Text title;
	Text menuoptions[5];
	bool optionsopen;
	int index;
	int volumelevel;
	Text vol;
	Texture menubg;
	Sprite menusp;
	Music  menumus;
	Sound hvsound;
	Sound selectsound;
	SoundBuffer selectsb;
	SoundBuffer hoversb;

public:
	Menu(float height, float width) : optionsopen(false), index(0), volumelevel(0) {
		if (!font.loadFromFile("font/arial.ttf")) {
			cout << "Error: Could not load font!" << endl;
			
		}

		if (!menubg.loadFromFile("Data/bg2.png")) {
			cout << "Error: Could not load texturre!" << endl;
		}
		menusp.setTexture(menubg);
		menusp.setScale(1.0f, 1.0f);
		title.setFont(font);
		title.setString("Sonic Classic Heroes");
		title.setStyle(Text::Bold);
		title.setCharacterSize(60);
		title.setFillColor(Color::Yellow);
		title.setPosition(width / 4, height / 10);
		vol.setFont(font);
		vol.setCharacterSize(30);
		vol.setFillColor(sf::Color::Cyan);
		vol.setPosition(width / 3, height - 150);
		voldisplay();

		menumus.openFromFile("Data/labrynth.ogg");
		menumus.setLoop(true);
		menumus.setVolume(volumelevel);  
		menumus.play();

		hoversb.loadFromFile("Data/MenuButton.ogg");
		hvsound.setBuffer(hoversb);
		selectsb.loadFromFile("Data/Select.ogg");
		selectsound.setBuffer(selectsb);

		string optionsarraay[5] = { "New Game", "Options", "Continue", "Leader Board", "Exit" };
		for (int i = 0;i < 5;i++) {
			menuoptions[i].setFont(font);
			menuoptions[i].setFillColor(Color::Blue);
			menuoptions[i].setString(optionsarraay[i]);
			menuoptions[i].setStyle(Text::Bold);
			menuoptions[i].setCharacterSize(40);
			menuoptions[i].setPosition(width / 3, height / 3 + i * 60);

		}
	
	}

	void draw(sf::RenderWindow& window) {
		window.draw(menusp); 

		if (!optionsopen) {
			window.draw(title);
			for (int i = 0; i < 5; ++i)
				window.draw(menuoptions[i]);
		}
		else {
			Text optTitle;
			optTitle.setFont(font);
			optTitle.setString("Options");
			optTitle.setCharacterSize(50);
			optTitle.setFillColor(Color::Green);
			optTitle.setPosition(300, 100);

			vol.setFont(font);  
			vol.setCharacterSize(30);
			vol.setFillColor(Color::Cyan);
			vol.setPosition(300, 250);



			

			window.draw(optTitle);
			window.draw(vol);


		}
	}

		void volset(int hilow) {
			if (hilow == 1) {
				if (volumelevel < 100) {
					volumelevel += 5;
					menumus.setVolume(volumelevel);
					voldisplay();
				}
			}
			else {
				if (volumelevel >0) {
					volumelevel -= 5;
					menumus.setVolume(volumelevel);
					voldisplay();
				}

			}
		}
		void voldisplay() {
			vol.setString("Volume: " + to_string(volumelevel));
		}
		void hower(int updown) {
			if (!optionsopen) {
			
				for (int i = 0; i < 5; i++) {
					menuoptions[i].setFillColor(Color::Blue);
				}
				index += updown;
				if (index < 0)
					index = 4;
				else if (index > 4)
					index = 0;

				
				menuoptions[index].setFillColor(Color::Magenta);
				hvsound.play();
			}
		}
		bool getoptionopen() const{
			return optionsopen;
			
		}
		int getindex() const {
			return index;
		}

		void optionselected(int index1) {
			selectsound.play();
			if (index1 == 0) {
				cout << "newgame" << endl;
				
			}
			else if (index1 == 1) {
				
				toggleing();

			}
			else if (index1 == 2) {
				cout << "toggle" << endl;

			}
			else if (index1 == 3) {
				cout << "toggle" << endl;

			}

		}

		void toggleing() {
			optionsopen = !optionsopen;
	}


	
};



int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Sonic Menu Test");
    Menu menu(896, 896);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

			if (event.type == Event::KeyPressed) {
				if (!menu.getoptionopen()) {
					if (event.key.code == sf::Keyboard::Up) {

						menu.hower(-1);
					}
					else if (event.key.code == sf::Keyboard::Down) {
						menu.hower(1);
					}
					else if (event.key.code == sf::Keyboard::Enter) {
						if (menu.getindex() == 4) {
							window.close();
						}
						else {
							menu.optionselected(menu.getindex());
						}
					}
				}

					else if (menu.getoptionopen()) {
						menu.draw(window);
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
        

		window.clear();
        menu.draw(window);
        window.display();
    }

	return 0;
}
