#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;

class PlayerFactory;
class PlayerManager;
class Player;
class Sonic;
class Tails;
class Knuckles;

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
	void setYposition(float y)
	{
		player_y = y;
	}
	virtual ~Player() = default;
	float getYPosition() const { return player_y; }
	void setYPosition(float y) { player_y = y; }
	void setOnGround(bool state) { onground = state; }
	void setVelocityY(float vy) { velocity_y = vy; }
	const Sprite& getSprite() const { return sprite; }

};

class SonicSpriteManager {
private:
	Texture idleRight;
	Texture idleLeft;
	Texture runRight;
	Texture runLeft;
	Texture jumpRight;
	Texture jumpLeft;
	Texture special;

public:
	SonicSpriteManager() 
	{
		idleRight.loadFromFile("Data/0right_still.png");
		idleLeft.loadFromFile("Data/0left_still.png");
		runRight.loadFromFile("Data/0jog_right.png");
		runLeft.loadFromFile("Data/0jog_left.png");
		jumpRight.loadFromFile("Data/0upR.png");
		jumpLeft.loadFromFile("Data/0upL.png");
		special.loadFromFile("Data/0right_run.png");
	}

	const Texture& getTexture(bool isMoving, bool isJumping, bool isFacingRight, bool isSpecial) const {
		if (isSpecial)
			return special;
		if (isJumping)
			return isFacingRight ? jumpRight : jumpLeft;
		if (isMoving)
			return isFacingRight ? runRight : runLeft;
		return isFacingRight ? idleRight : idleLeft;
	}
};


class Sonic :public Player {
	Clock abilityClock;
	bool boostActive;
	SonicSpriteManager spriteManager;
	bool moving;
	bool jumping;
	bool facingRight;
public:
	Sonic(float x, float y)
		:Player(18,0.5,x,y,-20,1), boostActive(false), moving(false),jumping(false), facingRight(true)
	{
		abilityTime = 15.0f;  // eans the ability would last 15 secinds
		abilityIsActive = false;
		velocity_x += acceleration;
		if (velocity_x > speed) //terminal velocity reached
			velocity_x = speed;

		setSprite();
		sprite.setTextureRect(IntRect(0,0,40,40));
		sprite.setScale(2.5f, 2.5f);
	}

	void setSprite() 
	{
		const Texture& currentTex = spriteManager.getTexture(moving,jumping,facingRight,boostActive);
		sprite.setTexture(currentTex);
	}
	virtual void moveRight() override
	{
		facingRight = true;
		moving = true;

		velocity_x += acceleration;
		if (velocity_x > speed)
			velocity_x = speed;

		setSprite();
	}
	virtual void moveLeft() override
	{
		facingRight = false;
		moving = true;

		velocity_x -= acceleration;
		if (velocity_x < -speed) 
			velocity_x = -speed;

		setSprite();

	}
	virtual void jump() override
	{
		if (onground) 
		{
			jumping = true;
			velocity_y = jump_strenght;
			onground = false;
			setSprite();
		}
	}
	virtual void useSpecialAbility() override
	{
		if (!boostActive) 
		{
			speed += 4;
			abilityClock.restart();
			boostActive = true;
			setSprite();
		}
		else 
		{
			// it disablea after 15 seconds
			if (abilityClock.getElapsedTime().asSeconds() >= 15.f) 
			{
				speed -= 4;
				boostActive = false;
			}
		}
	}
	virtual void setLeader(bool l)
	{
		isTheLeader = l;
	}
	virtual void updatePhysics() override 
	{
		Player::updatePhysics(); //for returning to ground

		if (onground) 
		{
			jumping = false;
			moving = (velocity_x != 0); //dependant on if theyre moving or not
			setSprite();
		}
	}
	virtual ~Sonic() override = default;

};


class TailsSpriteManager {
private:
	Texture idleRight;
	Texture idleLeft;
	Texture runRight;
	Texture runLeft;
	Texture jumpRight;
	Texture jumpLeft;
	Texture special;

public:
	TailsSpriteManager()
	{
		idleRight.loadFromFile("Data/tails.png");
		idleLeft.loadFromFile("Data/tails_l.png");
		runRight.loadFromFile("Data/run.png");
		runLeft.loadFromFile("Data/run_l.png");
		jumpRight.loadFromFile("Data/jump.png");
		jumpLeft.loadFromFile("Data/jump.png");
		special.loadFromFile("Data/fly.png");
	}

	const Texture& getTexture(bool isMoving, bool isJumping, bool isFacingRight, bool isSpecial) const {
		if (isSpecial)
			return special;
		if (isJumping)
			return isFacingRight ? jumpRight : jumpLeft;
		if (isMoving)
			return isFacingRight ? runRight : runLeft;
		return isFacingRight ? idleRight : idleLeft;
	}
};
class Tails : public Player {
	TailsSpriteManager spriteManager;
	Clock flightTime;
	bool flying;
	bool moving;
	bool jumping;
	bool facingRight;
public:
	Tails(float x, float y) : Player(10,0.3, x, y, -15, 0.8), flying(false), moving(false), jumping(false), facingRight(true) {}
	void setSprite()
	{
		const Texture& currentTex = spriteManager.getTexture(moving, jumping, facingRight, flying);
		sprite.setTexture(currentTex);
		sprite.setTextureRect(IntRect(0, 0, 40, 40));
		sprite.setScale(2.5f, 2.5f);
	}
	virtual void moveRight() override
	{
		facingRight = true;
		moving = true;

		velocity_x += acceleration;
		if (velocity_x > speed) 
			velocity_x = speed; //terminal velocity reached

		setSprite();
	}
	virtual void moveLeft() override
	{
		facingRight = false;
		moving = true;

		velocity_x -= acceleration;
		if (velocity_x < -speed)
			velocity_x = -speed;

		setSprite();
	}
	virtual void jump() override
	{
		if (onground)
		{
			jumping = true;
			velocity_y = jump_strenght;
			onground = false;
			setSprite();
		}
	}
	virtual void useSpecialAbility() override
	{
		if (!flying) 
		{
			velocity_y = -5;
			flightTime.restart();
			flying = true;
			setSprite();
		}
		else 
		{//manual table states 7 seconds while text states 4 seconds, impkemtned the 7s flight enhancement
			// stops flying after 7 seconds
			if (flightTime.getElapsedTime().asSeconds() >= 7.f) 
			{
				flying = false;
				setSprite();
			}
		}
	}
	virtual void setLeader(bool l)
	{
		isTheLeader = l;
	}
	virtual void updatePhysics() override 
	{
		Player::updatePhysics(); //for returning to ground after jumping

		if (onground)
		{
			jumping = false;
			moving = (velocity_x != 0); //dependant on if theyre moving or not
			setSprite();
		}
	}
	virtual ~Tails() override = default;
};


class KnucklesSpriteManager {
private:
	Texture idleRight;
	Texture idleLeft;
	Texture runRight;
	Texture runLeft;
	Texture jumpRight;
	Texture jumpLeft;
	Texture special;

public:
	KnucklesSpriteManager()
	{
		idleRight.loadFromFile("Data/knuckles.png");
		idleLeft.loadFromFile("Data/knuckles_l.png");
		runRight.loadFromFile("Data/run_knuckles.png");
		runLeft.loadFromFile("Data/run_knuckles_l.png");
		jumpRight.loadFromFile("Data/jump_k.png");
		jumpLeft.loadFromFile("Data/jump_k_l.png");
		special.loadFromFile("Data/dash.png");
	}

	const Texture& getTexture(bool isMoving, bool isJumping, bool isFacingRight, bool isSpecial) const {
		if (isSpecial)
			return special;
		if (isJumping)
			return isFacingRight ? jumpRight : jumpLeft;
		if (isMoving)
			return isFacingRight ? runRight : runLeft;
		return isFacingRight ? idleRight : idleLeft;
	}
};
class Knuckles : public Player {
Clock invincibilityClock;
KnucklesSpriteManager spriteManager;
bool invincible;
bool moving;
bool jumping;
bool facingRight;
	public:
		Knuckles(float x, float y) : Player(12, 0.4f, x, y, -18, 1.2),invincible(false), moving(false), jumping(false), facingRight(true) {}
		void setSprite()
		{
			const Texture& currentTex = spriteManager.getTexture(moving, jumping, facingRight, invincible);
			sprite.setTexture(currentTex);
			sprite.setTextureRect(IntRect(0, 0, 40, 40));
			sprite.setScale(2.5f, 2.5f);
		}
		virtual void moveRight() override
		{
			facingRight = true;
			moving = true;

			velocity_x += acceleration;
			if (velocity_x > speed)
				velocity_x = speed; //terminal velocity reached

			setSprite();
		}
		virtual void moveLeft() override
		{
			facingRight = false;
			moving = true;

			velocity_x -= acceleration;
			if (velocity_x < -speed)
				velocity_x = -speed;

			setSprite();
		}
		virtual void jump() override
		{
			if (onground)
			{
				jumping = true;
				velocity_y = jump_strenght;
				onground = false;
				setSprite();
			}
		}
		virtual void useSpecialAbility() override
		{
			if (!invincible) 
			{
				invincible = true;
				invincibilityClock.restart();
				setSprite();
			}
			else 
			{
				if (invincibilityClock.getElapsedTime().asSeconds() >= 15.f) //lasts for 15 seconds
				{
					invincible = false;
					setSprite();
				}
			}
		}
		virtual void updatePhysics() override
		{
			Player::updatePhysics(); //for returning to ground after jumping

			if (onground)
			{
				jumping = false;
				moving = (velocity_x != 0); //dependant on if theyre moving or not
				setSprite();
			}
		}
		virtual void setLeader(bool l)
		{
			isTheLeader = l;
		}
		virtual ~Knuckles() override = default;
};
class PlayerFactory {
public:
	Player* createPlayer(string type, float x, float y)
	{
		if (type == "Sonic")
			return new Sonic(x, y);
		if (type == "Tails")
			return new Tails(x, y);
		if (type == "Knuckles")
			return new Knuckles(x, y);
		return nullptr;
	}

};

class PlayerManager {
	Player* players[3]; //to be used for switching between the three characters
	int lives; //total lives
	int currentPlayer; //the array index for the leader
	float invincibilityTimer; //for when it gains invincibility 
	bool isInvincible;
public:
	PlayerManager(PlayerFactory& factory) :lives(3), currentPlayer(0)
	{
		players[0] = factory.createPlayer("Sonic", 100, 100);     //upcasting from child to parent
		players[1] = factory.createPlayer("Tails", 50, 100);
		players[2] = factory.createPlayer("Knuckles", 0, 100);
		players[0]->setLeader(true);
	}
	void updateFollowers()
	{
		Player& leader = *players[currentPlayer];
		float leader_x = leader.getXposition();
		float leader_y = leader.getYPosition();

		for (int i = 0;i < 3;i++)
		{
			if (i == currentPlayer)
				continue; //we dont want to implement the followers logic on the leading character
			
			Player* follower = players[i]; //set as a follower
			float follower_x = follower->getXposition();
			float displacement = leader_x - follower_x;

			//making sure te follwoers are alaways atleast 50px behind leader
			if (displacement > 60.0f)  
			{
				follower->moveRight(); 
			}
			else if (displacement < 40.0f) 
			{
				follower->moveLeft(); 
			}
			else
			{
				follower->setVelocityX(0); 
			}
			//applying jumping logic for follower

			float verticalDifference = leader.getYPosition() - follower->getYPosition();

			if (verticalDifference < -30.0f) 
			{
				follower->jump(); //kept a check of 30 px vertically, if 30 px difference then the follower also jumps agfter leader
			}

		}

	}
	~PlayerManager() {
		for (int i = 0; i < 3; ++i) 
		{
			delete players[i];
		}
	}
	Player* getLeader() const 
	{ 
		return players[currentPlayer]; 
	}
	Player* getPlayer(int i) const 
	{ 
		return players[i]; 
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


#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;

//int main()
//{
//	// Create game window
//	RenderWindow window(VideoMode(1280, 720), "Character Test");
//	window.setFramerateLimit(60);
//
//	PlayerFactory factory;
//	PlayerManager manager(factory);
//
//	Event ev;
//
//	while (window.isOpen())
//	{
//		while (window.pollEvent(ev))
//		{
//			if (ev.type == Event::Closed)
//				window.close();
//			if (ev.type == Event::KeyPressed)
//			{
//				if (ev.key.code == Keyboard::Tab)
//				{
//					manager.changePlayer(); // Switch character
//				}
//			}
//		}
//
//		// Input for leader only
//		if (Keyboard::isKeyPressed(Keyboard::Right))
//			manager.getLeader()->moveRight();
//		else if (Keyboard::isKeyPressed(Keyboard::Left))
//			manager.getLeader()->moveLeft();
//		else
//			manager.getLeader()->setVelocityX(0); // Stop if no key pressed
//
//		if (Keyboard::isKeyPressed(Keyboard::Space))
//			manager.getLeader()->jump();
//
//		if (Keyboard::isKeyPressed(Keyboard::LShift))
//			manager.getLeader()->useSpecialAbility();
//
//		// Update all players' physics
//		for (int i = 0; i < 3; ++i)
//		{
//			manager.getPlayer(i)->updatePhysics();
//
//			// Floor collision for each
//			if (manager.getPlayer(i)->getYPosition() >= 600)
//			{
//				manager.getPlayer(i)->setYPosition(600);
//				manager.getPlayer(i)->setVelocityY(0);
//				manager.getPlayer(i)->setOnGround(true);
//			}
//		}
//
//		// Update followers logic (sets velocity)
//		manager.updateFollowers();
//
//		// Rendering
//		window.clear(Color::Black);
//		for (int i = 0; i < 3; ++i)
//		{
//			const Sprite& sprite = manager.getPlayer(i)->getSprite();
//			Sprite s = sprite;
//			s.setPosition(manager.getPlayer(i)->getXposition(), manager.getPlayer(i)->getYPosition());
//			window.draw(s);
//		}
//		window.display();
//	}
//
//
//	return 0;
//}

