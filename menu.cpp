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
	bool canstart;
	string playerName;
public:
	Menu(int height, int width) : optionsopen(false), index(0), volumelevel(0), canstart(false) {
		if (!font.loadFromFile("font/arial.ttf")) {
			cout << "Error: Could not load font!" << endl;
		}
		if (!menubg.loadFromFile("Data/menubgg2.png")) {
			cout << "Error: Could not load texturre1!" << endl;
		}
		//initializing all the textures
		menusp.setTexture(menubg);
		menusp.setScale(1.0f, 1.25f);
		title.setFont(font);
		title.setString("SONIC CLASSIC HEROES");
		title.setStyle(Text::Bold);
		title.setCharacterSize(60);
		title.setFillColor(Color::Blue);
		title.setPosition(width - 260, height / 7);
		vol.setFont(font);
		vol.setCharacterSize(30);
		vol.setFillColor(sf::Color::Cyan);
		vol.setPosition(width / 2, height / 4);
		voldisplay();
		menumus.openFromFile("Data/labrynth.ogg");
		menumus.setLoop(true);
		menumus.setVolume(volumelevel);
		menumus.play();
		hoversb.loadFromFile("Data/MenuButton.ogg");
		hvsound.setBuffer(hoversb);
		selectsb.loadFromFile("Data/Select.ogg");
		selectsound.setBuffer(selectsb);
		// all menu options ka lia array hai and initalizing themm in a loop
		string optionsarraay[5] = { "New Game", "Options", "Continue", "Leader Board", "Exit" };
		for (int i = 0;i < 5;i++) {
			menuoptions[i].setFont(font);
			menuoptions[i].setFillColor(Color::Red);
			menuoptions[i].setString(optionsarraay[i]);
			menuoptions[i].setStyle(Text::Bold);
			menuoptions[i].setCharacterSize(40);
			menuoptions[i].setPosition(width - 100, (height) / 4 - 50 + i * 80);
		}
	}

	void draw(sf::RenderWindow& window, int height, int width) {
		window.draw(menusp);
		if (!optionsopen) { //if option menu is open tall menu options can be seen else if option is oprn volum be seen
			window.draw(title);
			for (int i = 0; i < 5; ++i) {
				window.draw(menuoptions[i]);
			}
		}
		else {
			Text optTitle;
			optTitle.setFont(font);
			optTitle.setString("Options");
			optTitle.setCharacterSize(50);
			optTitle.setFillColor(Color::Green);
			optTitle.setPosition(500, 200);
			vol.setFont(font);
			vol.setCharacterSize(30);
			vol.setFillColor(Color::Cyan);
			vol.setPosition(500, height / 2);
			Texture menn;
			Sprite menusp1;
			window.draw(optTitle);
			window.draw(vol);

		}
	} 
	// to set voll up and down
	void volset(int hilow) {
		if (hilow == 1) {
			if (volumelevel < 100) {
				volumelevel += 5;
				menumus.setVolume(volumelevel);
				voldisplay();
			}
		}
		else {
			if (volumelevel > 0) {
				volumelevel -= 5;
				menumus.setVolume(volumelevel);
				voldisplay();
			}

		}
	}
	// to display volume
	void voldisplay() {
		Texture menn;
		Sprite menusp1;
		if (!menn.loadFromFile("Data/menubgg2.png")) {
			cout << "Error: Could not load texturre2!" << endl;
		}
		menusp1.setTexture(menn);
		menusp1.setScale(5.0f, 5.0f);
		vol.setString("Volume: " + to_string(volumelevel));
	}
	// to handle  the navigation of the menu 
	void hower(int updown) {
		if (!optionsopen) { 
			for (int i = 0; i < 5; i++) {
				menuoptions[i].setFillColor(Color::Red);
			}
			index += updown; //up down
			if (index < 0)
				index = 4;
			else if (index > 4)
				index = 0;
			menuoptions[index].setFillColor(Color::Magenta);
			hvsound.play();
		}
	}
	bool getoptionopen() const {
		return optionsopen;
	}
	int getindex() const {
		return index;
	}
	// to take and setuo nae inputof user
	void nameinput(RenderWindow& window, int height, int width) {
		Text name;
		name.setFont(font);
		name.setString("Enter your name:");
		name.setCharacterSize(50);
		name.setFillColor(Color::Yellow);
		name.setPosition(width / 3 - 100, height / 2 - 100);
		// to display namee
		Text nameDisplay;
		nameDisplay.setFont(font);
		nameDisplay.setCharacterSize(50);
		nameDisplay.setFillColor(Color::White);
		nameDisplay.setPosition(width / 2 + 100, height / 2 - 100);
		// a loop to make another screen for name input
		while (window.isOpen()) {
			Event event;
			while (window.pollEvent(event)) {
				if (event.type == Event::Closed)
					window.close();
				// to handle how to let name entered
				if (event.type == Event::TextEntered) {
					if (event.text.unicode == 8 && playerName.length() > 0) {
						playerName.pop_back();
					}
					else if (event.text.unicode < 128) {
						playerName += static_cast<char>(event.text.unicode);
					}
				}
				if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
					canstart = true;
					return;
				}
				if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
					return;
				}
			}

			nameDisplay.setString(playerName);
			window.clear();
			window.draw(menusp);
			window.draw(name);
			window.draw(nameDisplay);
			window.display();
		}
	}
	bool getcanstart()
	{
		return canstart;
	}
	string getPlayerName() const { 
		return playerName; 
	}
	// to handle option slelected in menu
	void optionselected(int index1, RenderWindow& window, int height, int width) {
		selectsound.play();
		switch (index1) {
		case 0: //new game
			nameinput(window, height, width);
			break;
		case 1: //options
			toggleing();
			break;
		case 2: //continue
			cout << "Continue pressed!" << endl;
			break;
		case 3: //leaderboard
			cout << "Leader Board pressed!" << endl;
			break;
		case 4:
			exit(0);
		}
	}

	void toggleing() {
		optionsopen = !optionsopen;
	}



};