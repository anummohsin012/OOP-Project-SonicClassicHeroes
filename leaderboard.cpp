#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
using namespace sf;
using namespace std;

class Leaderboard {
private:
    static const int max= 10;
    string names[max];
    int scores[max];
    int count;
    Font font;

    void sortScores() {
        for (int i = 0; i < count - 1; i++) {
            for (int j = 0; j < count - i - 1; j++) {
                if (scores[j] < scores[j + 1]) {
                    int tempScore = scores[j];
                    scores[j] = scores[j + 1];
                    scores[j + 1] = tempScore;
                    string tempName = names[j];
                    names[j] = names[j + 1];
                    names[j + 1] = tempName;
                }
            }
        }
    }

public:
    Leaderboard() : count(0) {
        if (!font.loadFromFile("font/arial.ttf")) {
        }
        loadScores();
    }

    void addScore(const string& name, int score) {
        if (count <max) {
            names[count] = name;
            scores[count] = score;
            count++;
        }
        else {
            int minIndex = 0;
            for (int i = 1; i < max; i++) {
                if (scores[i] < scores[minIndex]) {
                    minIndex = i;
                }
            }
            if (score > scores[minIndex]) {
                names[minIndex] = name;
                scores[minIndex] = score;
            }
        }
        sortScores();
        saveScores();
    }

    void loadScores() {
        ifstream inFile("highscores.dat");
        count = 0;
        while (count < max && inFile >> names[count] >> scores[count]) {
            count++;
        }
        inFile.close();
        sortScores();
    }

    void saveScores() {
        std::ofstream outFile("highscores.dat");
        for (int i = 0; i < count; i++) {
            outFile << names[i] << " " << scores[i] << "\n";
        }
        outFile.close();
    }

    void draw(RenderWindow& window) {
        Text text;
        text.setFont(font);
        text.setCharacterSize(40);
        text.setFillColor(sf::Color::White);

        // Title
        text.setString("TOP 10 HIGH SCORES");
        text.setPosition(350, 100);
        window.draw(text);
        float y = 180;
        for (int i = 0; i < count; i++) {
            text.setString(to_string(i + 1) + ". " + names[i] + " - " + to_string(scores[i]));
            text.setPosition(350, y);
            window.draw(text);
            y += 50;
        }
    }
};
