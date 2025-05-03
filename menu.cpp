#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;
using namespace sf;

class Menu {
private:
	Font font;
	Text title;
	Text menuoptions[5];
	bool optionsopen;
	int index = 0;
	int volumelevel;
	Text vol;
	Texture menubg;
	Sprite menusp;
public:
	Menu()
};