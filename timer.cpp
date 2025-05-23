#include <SFML/System.hpp>
using namespace sf;

class Timer {
private:
    Clock clock;
    float duration;
    bool working;
public:
    Timer(float dur1=0.0f):duration(dur1),working(false){}
    void start() {
        clock.restart();
        working = true;
    }
    void setDura(float time) {
        duration = time;
    }
    void stop() {
        working = false;
    }
    bool timeup() const {
        return working && clock.getElapsedTime().asSeconds() >= duration;
    }
    float getRemainingTime() const {
        float remaining = duration - clock.getElapsedTime().asSeconds();
        if (remaining < 0.f) {
            remaining = 0.f;
        }
        return remaining;
    }

};