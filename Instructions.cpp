#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
using namespace sf;
using namespace std;

class Instructions {
private:
    Font font;
    Text heading;
    Text instructionText;
    string content;

public:
    Instructions(float width, float height) {
        if (!font.loadFromFile("font/arial.ttf")) {
        }

        // Load file manually, no streams
        ifstream file("Instructions.txt.txt");
        if (!file.is_open()) {
          
            content = "No instructions available.";
        }
        else {
            char line[256];
            while (!file.eof()) {
                file.getline(line, 256);
                content += line;
                content += '\n';
            }
            file.close();
        }

        // Heading setup
        heading.setFont(font);
        heading.setString("Game Manual");
        heading.setCharacterSize(50);
        heading.setFillColor(Color::Yellow);
        heading.setStyle(Text::Bold);
        heading.setPosition(width / 2 - 150, 20);  // Adjusted center

        // Instruction text setup
        instructionText.setFont(font);
        instructionText.setString(content);
        instructionText.setCharacterSize(36);
        instructionText.setFillColor(Color::White);
        instructionText.setPosition(50, 100);  // Leave space for heading
    }

    void draw(RenderWindow& window) {
        window.draw(heading);
        window.draw(instructionText);
    }
};
