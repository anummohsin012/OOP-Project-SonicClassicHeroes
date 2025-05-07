#include <SFML/System.hpp>
using namespace sf;

class Timer {
private:
    sf::Clock clock;
    float duration;
    bool working;
public:
    Timer(float dur1=0.0f):duration(dur1),working(false){}
    void start() {
        clock.restart();
        working = true;
    }
    void stop() {
        working = false;
    }
    void setDura(float time) {
        duration = time;
    }
    bool timeup() const {
        return working && clock.getElapsedTime().asSeconds() >= duration;
    }
    float getRemainingTime() const {
        float remaining = duration - clock.getElapsedTime().asSeconds();
        if (remaining < 0.f) remaining = 0.f;
        return remaining;
    }

};