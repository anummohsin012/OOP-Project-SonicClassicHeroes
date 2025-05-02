using namespace std;
class ScoreBoard {
	int score;
public:
	ScoreBoard() :score(0) {}
	int getscore()
	{
		return score;
	}
	void addPoints(int s)
	{
		score += s;
	}
	void removePoints(int s)
	{
		score -= s;
	}
};
class Game {
private:
	ScoreBoard Score;

};
