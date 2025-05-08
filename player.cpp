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
	int height, width; //of level
public:
	Player(float s,float a, float x, float y, float jump, float gr,int h,int w)
		:speed(s),acceleration(a),player_x(x),player_y(y),velocity_x(0),velocity_y(0),jump_strenght(jump),gravity(gr), height(h),width(w){ }
	virtual void moveRight(char** lvl, float cell_size) = 0;
	virtual void moveLeft(char** lvl, float cell_size) = 0;
	virtual void jump() = 0;
	virtual void useSpecialAbility() = 0;

	//hitboxes 
//keeping the logic that our sprites are originally 40px by 40 px and then keeping offsets of 5 from each direction making it 30 by 30
	const float HITBOX_LEFT = 0.f;
	const float HITBOX_RIGHT = (40*2.5f);
	const float HITBOX_TOP = 0.f;
	const float HITBOX_BOTTOM = (40*2.5f);

	//functions where we apply physics

	void updatePhysicsWithCollision(char** lvl, int cell_size) {
		//finding midpoints
		const float vertical_mid = (HITBOX_TOP + HITBOX_BOTTOM) / 2;
		const float horizontal_mid = (HITBOX_LEFT + HITBOX_RIGHT) / 2;
		
		//velocity
		float next_x = player_x + velocity_x;
		float next_y = player_y + velocity_y;

		//using source logic to check for collisions 
		int left_col =(int)(next_x + HITBOX_LEFT) / cell_size;
		int right_col =  (int)(next_x + HITBOX_RIGHT) / cell_size; 
		int mid_row =  (int)(player_y + vertical_mid) / cell_size;

		//checking bounds
		if (mid_row >= 0 && mid_row < height) 
		{

			if (left_col >= 0 && left_col < width && lvl[mid_row][left_col] == 'w')  //collision from left
			{
				velocity_x = 0;  //stop and freeze to grid boundary
				player_x = (left_col + 1) * cell_size - HITBOX_LEFT;
			}
			else if (right_col >= 0 && right_col < width && lvl[mid_row][right_col] == 'w')
			{
				velocity_x = 0;
				player_x = right_col * cell_size - HITBOX_RIGHT;
			}
			else 
			{
				player_x = next_x;
			}
		}
		int top_row = (int)(next_y + HITBOX_TOP) / cell_size;
		int bottom_row = (int)(next_y + HITBOX_BOTTOM) / cell_size;
		int mid_col = (int)(player_x + horizontal_mid) / cell_size;

		//  bounds
		if (mid_col >= 0 && mid_col < width)
		{
			if (bottom_row >= 0 && bottom_row < height && (lvl[bottom_row][mid_col] == 'w'|| lvl[bottom_row][mid_col] == 's'))
			{
				player_y = bottom_row * cell_size - HITBOX_BOTTOM; //snap to ground
				velocity_y = 0;
				onground = true;
			}
			else if (top_row >= 0 && top_row < height && (lvl[bottom_row][mid_col] == 'w' || lvl[bottom_row][mid_col] == 's'))
			{
				velocity_y = 0; //cieling hit
			}
			else
			{
				player_y = next_y;
				onground = false;
			}
		}
	}
	virtual void updatePhysics() 
	{
		//for jumping
		player_x += velocity_x;
		player_y += velocity_y;
		if (!onground)
			velocity_y += gravity;
	}

	//dealing with positions
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
	float getYPosition() const 
	{ 
		return player_y; 
	}
	void setYPosition(float y) 
	{ 
		player_y = y; 
	}
	void setOnGround(bool state) 
	{ 
		onground = state; 
	}
	void setVelocityY(float vy) 
	{ 
		velocity_y = vy; 
	}
	const Sprite& getSprite() const 
	{ 
		return sprite; 
	}
	void respawn(float leader_x, float leader_y) 
	{
		player_x = leader_x+50.0; //follower spawns next to leader as according to original logic
		player_y = leader_y;
		velocity_x = 0;
		velocity_y = 0;
		onground = true;
	}
	virtual bool isInvincible() = 0;
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
	Sonic(float x, float y, int h, int w)
		:Player(18,0.5,x,y,-20,1, h,w), boostActive(false), moving(false),jumping(false), facingRight(true)
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
		const Texture& current = spriteManager.getTexture(moving,jumping,facingRight,boostActive);
		sprite.setTexture(current);
	}

	virtual void moveRight(char** lvl, float cell_size) override 
	{
		// checking for the wall
		int right_col = (int)(player_x + HITBOX_RIGHT + speed) / cell_size;
		int mid_row = (int)(player_y + (HITBOX_TOP + HITBOX_BOTTOM) / 2) / cell_size;

		//bounds
		if (mid_row >= 0 && mid_row < height && right_col >= 0 && right_col < width) 
		{
			if (lvl[mid_row][right_col] != 'w') 
			{
				facingRight = true;
				moving = true;
				velocity_x = speed;
				setSprite();
			}
		}
	}
	void moveLeft(char** lvl, float cell_size) override 
	{
		int left_col = (int)(player_x + HITBOX_LEFT - speed) / cell_size; 
		int mid_row = (int)(player_y + (HITBOX_TOP + HITBOX_BOTTOM) / 2) / cell_size;
		if (mid_row >= 0 && mid_row < height && left_col >= 0 && left_col < width)
		{
			if (lvl[mid_row][left_col] != 'w') 
			{
				facingRight = false;  
				moving = true;
				velocity_x = -speed; 
				setSprite();
			}
		}
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
			sprite.setTextureRect(IntRect(80, 0, 40, 40));
			setSprite();
		}
	}
	virtual void setLeader(bool l)
	{
		isTheLeader = l;
	}
	virtual void updatePhysics() override 
	{
		if (boostActive && abilityClock.getElapsedTime().asSeconds() >= 15.f) {
			speed = 18.f; 
			boostActive = false;
			sprite.setTextureRect(IntRect(0, 0, 40, 40));
			setSprite();
		}

		
		Player::updatePhysics(); //for returning to ground

		if (onground) 
		{
			jumping = false;
			moving = (velocity_x != 0); //dependant on if theyre moving or not
			setSprite();
		}
	}
	virtual ~Sonic() override{}
	virtual bool isInvincible() override
	{
		return false;
	}

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
	float flight_y;
	bool wasFlying;
public:
	Tails(float x, float y,int h, int w) : Player(10,0.3, x, y, -15, 0.8,h,w), flying(false), moving(false), jumping(false), facingRight(true), wasFlying(false) {}
	void setSprite()
	{
		const Texture& currentTex = spriteManager.getTexture(moving, jumping, facingRight, flying);
		sprite.setTexture(currentTex);
		sprite.setTextureRect(IntRect(0, 0, 40, 40));
		sprite.setScale(2.5f, 2.5f);
	}
	virtual void moveRight(char** lvl, float cell_size) override
	{
		// checking for the wall
		int right_col = (int)(player_x + HITBOX_RIGHT + speed) / cell_size;
		int mid_row = (int)(player_y + (HITBOX_TOP + HITBOX_BOTTOM) / 2) / cell_size;

		//bounds
		if (mid_row >= 0 && mid_row < height && right_col >= 0 && right_col < width)
		{
			if (lvl[mid_row][right_col] != 'w')
			{
				facingRight = true;
				moving = true;
				velocity_x = speed;
				setSprite();
			}
		}
	}
	void moveLeft(char** lvl, float cell_size) override
	{
		int left_col = (int)(player_x + HITBOX_LEFT - speed) / cell_size;
		int mid_row = (int)(player_y + (HITBOX_TOP + HITBOX_BOTTOM) / 2) / cell_size;
		if (mid_row >= 0 && mid_row < height && left_col >= 0 && left_col < width)
		{
			if (lvl[mid_row][left_col] != 'w')
			{
				facingRight = false;
				moving = true;
				velocity_x = -speed;
				setSprite();
			}
		}
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
			flight_y = player_y;
			velocity_y = -5;
			flightTime.restart();
			flying = true;
			setSprite();
		}
	}
	virtual void setLeader(bool l)
	{
		isTheLeader = l;
	}
	virtual void updatePhysics() override 
	{
		if (flying) 
		{
			//dependant on flight duration
			if (flightTime.getElapsedTime().asSeconds() >= 7.f ) 
			{
				flying = false;
			}
			// Apply reduced upward velocity
			if (flying) 
			{
				if ((player_y <= flight_y - 300.f))
				{
					velocity_y =+3;  //to keep it at 300 px
				}
				else
				{
					velocity_y=-3; //moves up
				}
			}
			else 
			{
				velocity_y = 0;  // Stop upward motion once timer stops
			}
		}
		// gravity when on the floor or jumping
		if (!flying) 
		{
			velocity_y += gravity;
		}

		Player::updatePhysics();

		if (onground) 
		{
			jumping = false;
			moving = (velocity_x != 0);
			setSprite();
		}
	}
	virtual ~Tails() override{}
	virtual bool isInvincible() override
	{
		return false;
	}
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
		Knuckles(float x, float y, int h, int w) : Player(12, 0.4f, x, y, -18, 1.2,h,w),invincible(false), moving(false), jumping(false), facingRight(true) {}
		void setSprite()
		{
			const Texture& currentTex = spriteManager.getTexture(moving, jumping, facingRight, invincible);
			sprite.setTexture(currentTex);
			sprite.setTextureRect(IntRect(0, 0, 40, 40));
			sprite.setScale(2.5f, 2.5f);
		}
		virtual void moveRight(char** lvl, float cell_size) override
		{
			// checking for the wall
			int right_col = (int)(player_x + HITBOX_RIGHT + speed) / cell_size;
			int mid_row = (int)(player_y + (HITBOX_TOP + HITBOX_BOTTOM) / 2) / cell_size;

			//bounds
			if (mid_row >= 0 && mid_row < height && right_col >= 0 && right_col < width)
			{
				if (lvl[mid_row][right_col] != 'w')
				{
					facingRight = true;
					moving = true;
					velocity_x = speed;
					setSprite();
				}
			}
		}
		virtual void moveLeft(char** lvl, float cell_size) override
		{
			int left_col = (int)(player_x + HITBOX_LEFT - speed) / cell_size;
			int mid_row = (int)(player_y + (HITBOX_TOP + HITBOX_BOTTOM) / 2) / cell_size;
			if (mid_row >= 0 && mid_row < height && left_col >= 0 && left_col < width)
			{
				if (lvl[mid_row][left_col] != 'w')
				{
					facingRight = false;
					moving = true;
					velocity_x = -speed;
					setSprite();
				}
			}
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
		virtual void useSpecialAbility() override //WORK ON CLOCK NOTE TO SELF SET KNUCKLES SPECIAL ABILITY
		{
			if (!invincible) 
			{
				invincible = true;
				invincibilityClock.restart();
				setSprite();
			}
		}
		virtual void updatePhysics() override
		{
			if (invincible && invincibilityClock.getElapsedTime().asSeconds() >= 15.f) 
			{
				invincible = false;
				setSprite();
			}
			
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
		virtual ~Knuckles() override{}
		virtual bool isInvincible() override
		{
			return invincible;
		}
};
class PlayerFactory {
public:
	Player* createPlayer(string type, float x, float y, int h, int w)
	{
		if (type == "Sonic")
			return new Sonic(x, y,h,w);
		if (type == "Tails")
			return new Tails(x, y, h ,w);
		if (type == "Knuckles")
			return new Knuckles(x, y, h,w);
		return nullptr;
	}

};

class PlayerManager {
	Player* players[3]; //to be used for switching between the three characters
	int lives; //total lives
	int currentPlayer; //the array index for the leader
	const float pitThreshold;
	bool gameover;
	char** lvl;
	float cell_size;
	int height, width;
public:
	PlayerManager(PlayerFactory& factory,char** lvl, int h, int w) :lives(3), currentPlayer(0), pitThreshold(800), gameover(false),lvl(lvl),cell_size(64),height(h),width(w)
	{
		players[0] = factory.createPlayer("Sonic", 100, 100,h,w);     //upcasting from child to parent
		players[1] = factory.createPlayer("Tails", 50, 100,h,w);
		players[2] = factory.createPlayer("Knuckles", 0, 100,h,w);
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
				follower->moveRight(lvl,cell_size); 
			}
			else if (displacement < 40.0f) 
			{
				follower->moveLeft(lvl,cell_size); 
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
	int getLives()
	{
		return lives;
	}
	void addLife()
	{
		lives += 1;
	}
	void removeLife()
	{
		if (lives<=0)
		{
			gameover = true;
			return;
		}
		else
		lives -= 1;
	}
	void checkPitRespawns() {
		Player* leader = getLeader();

		// Convert leader position to grid coordinates
		int leader_col = leader->getXposition() / cell_size;
		int leader_row = leader->getYPosition() / cell_size;

		// Check if leader touched any pit cells (column 13 in this case)
		if (leader_row >= 0 && leader_row < height && leader_col >= 0 && leader_col < width)
		{
			if (lvl[leader_row][leader_col] == 'p') 
			{
				removeLife();
				if (!gameover) 
				{
					// Respawn all players
					float respawnX = max(100.f, leader->getXposition() - 200.f);
					for (int i = 0; i < 3; ++i) {
						players[i]->respawn(respawnX, 100.f);
						players[i]->setVelocityX(0);
						players[i]->setVelocityY(0);
						players[i]->setOnGround(true);
					}
				}
				return;
			}
		}

		// Followers (existing behavior)
		for (int i = 0; i < 3; ++i) {
			if (i != currentPlayer) {
				int follower_row = players[i]->getYPosition() / cell_size;
				int follower_col = players[i]->getXposition() / cell_size;

				if (follower_row >= 0 && follower_row < height &&
					follower_col >= 0 && follower_col < width &&
					lvl[follower_row][follower_col] == 'p')
				{
					players[i]->respawn(leader->getXposition(), 100.f);
				}
			}
		}
	}

	// Remove leaderPitcheck() entirely - redundant
	void leaderPitcheck(Player* p)
	{
		if (p->getYPosition() > pitThreshold) //for incase the leader falls into the pit 
		{
			p->respawn(p->getXposition() + 64, 100); //goes back a cell

		}
	}
	bool isGameOver()
	{
		return gameover;
	}
};
