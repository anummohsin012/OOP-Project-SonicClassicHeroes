#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>

class Leaderboard {
private:
    static const int MAX_SCORES = 10;
    std::string names[MAX_SCORES];
    int scores[MAX_SCORES];
    int scoreCount;
    sf::Font font;

    void sortScores() {
        // Bubble sort without using swap
        for (int i = 0; i < scoreCount - 1; i++) {
            for (int j = 0; j < scoreCount - i - 1; j++) {
                if (scores[j] < scores[j + 1]) {
                    // Manual swap for scores
                    int tempScore = scores[j];
                    scores[j] = scores[j + 1];
                    scores[j + 1] = tempScore;

                    // Manual swap for names
                    std::string tempName = names[j];
                    names[j] = names[j + 1];
                    names[j + 1] = tempName;
                }
            }
        }
    }

public:
    Leaderboard() : scoreCount(0) {
        if (!font.loadFromFile("font/arial.ttf")) {
            // Error handling
        }
        loadScores();
    }

    void addScore(const std::string& name, int score) {
        if (scoreCount < MAX_SCORES) {
            names[scoreCount] = name;
            scores[scoreCount] = score;
            scoreCount++;
        }
        else {
            // Find the lowest score
            int minIndex = 0;
            for (int i = 1; i < MAX_SCORES; i++) {
                if (scores[i] < scores[minIndex]) {
                    minIndex = i;
                }
            }

            // Replace if new score is higher
            if (score > scores[minIndex]) {
                names[minIndex] = name;
                scores[minIndex] = score;
            }
        }
        sortScores();
        saveScores();
    }

    void loadScores() {
        std::ifstream inFile("highscores.dat");
        scoreCount = 0;
        while (scoreCount < MAX_SCORES && inFile >> names[scoreCount] >> scores[scoreCount]) {
            scoreCount++;
        }
        inFile.close();
        sortScores();
    }

    void saveScores() {
        std::ofstream outFile("highscores.dat");
        for (int i = 0; i < scoreCount; i++) {
            outFile << names[i] << " " << scores[i] << "\n";
        }
        outFile.close();
    }

    void draw(sf::RenderWindow& window) {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(40);
        text.setFillColor(sf::Color::White);

        // Title
        text.setString("TOP 10 HIGH SCORES");
        text.setPosition(350, 100);
        window.draw(text);

        // Scores
        float y = 180;
        for (int i = 0; i < scoreCount; i++) {
            text.setString(std::to_string(i + 1) + ". " + names[i] + " - " + std::to_string(scores[i]));
            text.setPosition(350, y);
            window.draw(text);
            y += 50;
        }
    }
};
