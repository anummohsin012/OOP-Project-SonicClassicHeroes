#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
using namespace sf;
using namespace std;


class playerManager {
	Player* players[3]; //to be used for switching between the three characters
	int lives; //total lives
	int currentPlayer; //the array index for the leader
	float invincibilityTimer; //for when it gains invincibility 
	bool isInvincible;
public:
	playerManager(PlayerFactory& factory) :lives(3), currentPlayer(0)
	{
		players[0] =factory.createPlayer("Sonic",100,100);     //upcasting from child to parent
		players[1] = factory.createPlayer("Tails",50,100);
		players[2] = factory.createPlayer("Knuckles",0,100);
		players[0]->setLeader(true);
	}
	void updateFollowers()
	{
		

		
	}

	void changePlayerIndex() 
	{
		currentPlayer = (currentPlayer + 1) % 3; //shifting to the next player index
	}

	Player& changePlayer()
	{
		changePlayerIndex(); //updating the index and returning that player
		return *players[currentPlayer];
	}
};
class PlayerFactory {
public:
	Player* createPlayer(string type, float x, float y)
	{
		if (type == "Sonic")    
			return new Sonic(x,y);
		if (type == "Tails")    
			return new Tails(x,y);
		if (type == "Knuckles") 
			return new Knuckles(x,y);
		return nullptr;
	}

};
class Player {
protected:
	float speed; //general max speed 
	float player_x; //horizontal position
	float player_y; //vertical postion
	float velocity_x; //horizontal velocity
	float velocity_y; //vertical velocity
	float jump_strenght; //how high the player can jump
	float gravity; //how fast it comes down
	Texture texture;
	Sprite sprite;
	bool onground;
	bool isTheLeader;
	bool died;
	bool damaged;
public:
	Player(float s, float x, float y, float jump, float gr)
		:speed(s),player_x(x),player_y(y),velocity_x(0),velocity_y(0),jump_strenght(jump),gravity(gr){ }
	virtual void moveRight() = 0;
	virtual void moveLeft() = 0;
	virtual void jump() = 0;
	virtual void useSpecialAbility() = 0;
	virtual ~Player(){}

	//functions where we apply physics
	void updatePhysics() 
	{
		if (!onground)
			velocity_y += gravity;
		player_x += velocity_x;
		player_y += velocity_y;
	}
	virtual void setLeader(bool) = 0;
};
class Sonic :public Player {
public:
	Sonic(float x, float y):Player(18,x,y,0,0,-20,1)
	{
		texture.loadFromFile("Data/0right_still.png");
		sprite.setTexture(texture);
	}
	virtual void moveRight() override
	{
		texture.loadFromFile("Data/0right.png");
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(40, 40, 40, 40));

	}
	virtual void moveLeft() override
	{
		texture.loadFromFile("Data/0left.png");
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(0, 0, 40, 40));
	}
	virtual void jump() override
	{
		texture.loadFromFile("Data/0up_L.png");
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(40, 40, 40, 40));
	}
	virtual void useSpecialAbility() override
	{
		speed = 15;              
		if (Keyboard::isKeyPressed(Keyboard::Right))
		{
            texture.loadFromFile("Data/0right_run.png");
			sprite.setTexture(texture);
			sprite.setTextureRect(IntRect(40, 40, 40, 40));
			moveRight();
		}
		else if (Keyboard::isKeyPressed(Keyboard::Left))
		{
			texture.loadFromFile("Data/0left_run.png");
			sprite.setTexture(texture);
			sprite.setTextureRect(IntRect(40, 40, 40, 40));
			moveLeft();
		}
	}
	virtual void setLeader(bool l)
	{
		isTheLeader = l;
	}

};
class Tails : public Player {

public:
	virtual void moveRight() override
	{

	}
	virtual void moveLeft() override
	{

	}
	virtual void jump() override
	{

	}
	virtual void useSpecialAbility() override
	{

	}
	virtual void setLeader(bool l)
	{
		isTheLeader = l;
	}

};
class Knuckles : public Player {

public:
	virtual void moveRight() override
	{

	}
	virtual void moveLeft() override
	{

	}
	virtual void jump() override
	{

	}
	virtual void useSpecialAbility() override
	{

	}
	virtual void setLeader(bool l)
	{
		isTheLeader = l;
	}

};