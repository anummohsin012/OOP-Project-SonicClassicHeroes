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
		Player& leader = *players[currentPlayer];

		for (int i = 0;i < 3;i++)
		{
			if (i == currentPlayer)
				continue; //we dont want to implement the followers logic on the leading character
			Player* follower = players[i]; //set as a follower
			//set on the direction of following
			float leaderDirection = (leader.getVelocityX() > 0) ? 1.0f : -1.0f; //positive means right 
			//making sure the followers stay 50px behind the leader
			float targetX = leader.getXposition() + (leaderDirection * -50.0f);
			float dx = targetX - follower->getXposition();
			float followSpeed = follower->getSpeed() * 0.6f; //the speed of followers will also be 0.6 of the original to make them slow

			//dealing with edge cases and setting the velocities
			if (dx > 0) //for moving towards leader
			{
				float desiredSpeed = dx * 0.15f;
				if (desiredSpeed > followSpeed) 
				{
					follower->setVelocityX(followSpeed); //if our desried speech is gereater than the max follower speed set
				}
				else 
				{
					follower->setVelocityX(desiredSpeed);
				}
			}
			else if (dx < 0) //moving back to leader
			{
				float desiredSpeed = dx * 0.15f;
				if (desiredSpeed < -followSpeed) 
				{
					follower->setVelocityX(-followSpeed);
				}
				else 
				{
					follower->setVelocityX(desiredSpeed); // when desired speech is larger for quick replacemnt
				}
			}
		}

	}

	void changePlayer() 
	{
		players[currentPlayer]->setLeader(false);
		currentPlayer = (currentPlayer + 1) % 3; //shifting to the next player index
		players[currentPlayer]->setLeader(true); //making the next one the player leader
	}

	//Player& changePlayer()
	//{
	//	changePlayerIndex(); //updating the index and returning that player
	//	return *players[currentPlayer];
	//}
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
	float acceleration;
	Clock abilityClock;
	float abilityTime;
	bool abilityIsActive;
public:
	Player(float s,float a, float x, float y, float jump, float gr)
		:speed(s),acceleration(a),player_x(x),player_y(y),velocity_x(0),velocity_y(0),jump_strenght(jump),gravity(gr){ }
	virtual void moveRight() = 0;
	virtual void moveLeft() = 0;
	virtual void jump() = 0;
	virtual void useSpecialAbility() = 0;
	virtual ~Player(){}

	//functions where we apply physics
	virtual void updatePhysics() 
	{
		player_x += velocity_x;
		player_y += velocity_y;
		if (!onground)
			velocity_y += gravity;
	}
	virtual void setLeader(bool) = 0;
	float getXposition() const
	{
		return player_x;
	}
	float getVelocityX() const 
	{ 
		return velocity_x; 
	}
	float getSpeed() const 
	{ 
		return speed; 
	}
	void setVelocityX(float vx) 
	{ 
		velocity_x = vx; 
	}

};
class Sonic :public Player {
public:
	Sonic(float x, float y):Player(18,0.5,x,y,-20,1)
	{
		abilityTime = 15.0f;  // eans the ability would last 15 secinds
		abilityIsActive = false;
		texture.loadFromFile("Data/0right_still.png");
		sprite.setTexture(texture);
		velocity_x += acceleration;
		if (velocity_x > speed) //terminal velocity reached
			velocity_x = speed;
	}
	virtual void moveRight() override
	{
		texture.loadFromFile("Data/0right_run.png");
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(40, 40, 40, 40));
		velocity_x -= acceleration;
		if (velocity_x < -speed)
			velocity_x = -speed;

	}
	virtual void moveLeft() override
	{
		texture.loadFromFile("Data/0left_run.png");
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(40, 40, 40, 40));
	}
	virtual void jump() override
	{
		texture.loadFromFile("Data/0up_L.png");
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(40, 40, 40, 40));
	}
	virtual void useSpecialAbility() override
	{
		speed +=4;              
		if (Keyboard::isKeyPressed(Keyboard::Right))
		{
			moveRight();
		}
		else if (Keyboard::isKeyPressed(Keyboard::Left))
		{
			moveLeft();
		}
	}
	virtual void setLeader(bool l)
	{
		isTheLeader = l;
	}

};
class Tails : public Player {
	float flightTimer;
public:
	Tails(float x, float y) : Player(10,0.3, x, y, -15, 0.8), flightTimer(0) {}
	virtual void moveRight() override
	{
		velocity_x += acceleration;
		if (velocity_x > speed) //terminal velocity reached
			velocity_x = speed;
	}
	virtual void moveLeft() override
	{
		velocity_x -= acceleration;
		if (velocity_x < -speed)
			velocity_x = -speed;
	}
	virtual void jump() override
	{

	}
	virtual void useSpecialAbility() override
	{
		if (flightTimer <= 0) //so tails can fly for 7 seconds
			flightTimer = 7.0f;
	}
	virtual void setLeader(bool l)
	{
		isTheLeader = l;
	}
	virtual void updatePhysics() override 
	{
		if (flightTimer > 0)
		{
			velocity_y = -5; //so that tails moves upwards
			flightTimer -= 1 / 60.f;
		}
		Player::updatePhysics(); //change the physics of the player
	}
};
class Knuckles : public Player {
	public:
		Knuckles(float x, float y) : Player(12, 0.4f, x, y, -18, 1.2) {}
		virtual void moveRight() override
		{
			velocity_x += acceleration;
			if (velocity_x > speed) 
				velocity_x = speed;
		}
		virtual void moveLeft() override
		{
			velocity_x -= acceleration;
			if (velocity_x < -speed) 
				velocity_x = -speed;
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