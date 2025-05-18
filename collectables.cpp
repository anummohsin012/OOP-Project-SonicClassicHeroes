#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;

class Collectibles { //parent class
protected: 
	Texture texture;
	Sprite sprite;
	int row, column;
	SoundBuffer collectSound;
	bool soundLoaded;
public:
	Collectibles(int r, int c) :row(r), column(c) {}
	void draw(float cell_size, RenderWindow& window, float offset)
	{
		sprite.setPosition(column * cell_size - offset, getY(cell_size));
		window.draw(sprite);
	}
	virtual void collect(char** lvl) = 0;
	virtual ~Collectibles(){}
	virtual void playSound() = 0;

	float getX(float cell_size) const 
	{ 
		return column * cell_size; 
	}
	float getY(float cell_size) const 
	{ 
		return row * cell_size; 
	}
	virtual string typeOfCollectible() = 0;
};

class Rings:public Collectibles{
	
public:
	Rings(int r,int c, char** lvl):Collectibles(r,c)
	{
		texture.loadFromFile("Data/ring.png");
		sprite.setTexture(texture);
		sprite.setScale(1.5f, 1.5f);

		lvl[r][c] = 'R';
	}
	virtual void collect(char** lvl)
	{
		lvl[row][column] = ' ';
		playSound();
	}
	virtual void playSound() 
	{

		static Music collisionmusic;
		collisionmusic.openFromFile("Data/Ring.wav");
		collisionmusic.setVolume(60);
		collisionmusic.play();
	}
	~ Rings(){}
	virtual string typeOfCollectible()
	{
		return "ring";
	}
};

class ExtraLives: public Collectibles {

public:
	ExtraLives(int r, int c, char** lvl) : Collectibles(r, c)
	{
		texture.loadFromFile("Data/life.png");
		sprite.setTexture(texture);
		sprite.setScale(3.0f, 3.0f);
		lvl[r][c] = 'L';
	}
	
	virtual void collect(char** lvl)
	{
		lvl[row][column] = ' ';
		playSound();
	}
	~ ExtraLives(){}
	virtual void playSound() {
		static Music collisionmusic;
		collisionmusic.openFromFile("Data/LargeBooster.wav");
		collisionmusic.setVolume(60);
		collisionmusic.play();
	}
	virtual string typeOfCollectible()
	{
		return "extralife";
	}
};


class SpecialAbility: public Collectibles {

public:
	SpecialAbility(int r, int c, char** lvl) : Collectibles(r, c)
	{
		texture.loadFromFile("Data/bonus.png");
		sprite.setTexture(texture);
		sprite.setScale(3.0f,3.0f);
		lvl[r][c] = 'A';
	}
	
	virtual void collect(char** lvl)
	{
		lvl[row][column] = ' ';
		playSound();
	}
	virtual void playSound() {
		static Music collisionmusic;
		collisionmusic.openFromFile("Data/HLaser.wav");
		collisionmusic.setVolume(60);
		collisionmusic.play();
	}
	~SpecialAbility(){}
	virtual string typeOfCollectible()
	{
		return "specialability";
	}
};


class CollectibleFactory {
	int size;
	Collectibles** collectibles; //composing the collectible objects because it creates collectibles
	int current;
public:
	CollectibleFactory(int size = 3) :size(size), collectibles(new Collectibles* [size]),current(0)
	{
		for (int i = 0;i < size;i++)
			collectibles[i] = nullptr;
	}
	void spawn(Collectibles* c)
	{
		if (current < size)
		{
			collectibles[current++] = c;
		}
		else
		{
			resize();
			collectibles[current++] = c;
		}
	}
private:
	void resize()
	{
		int tempsize = size * 2; //dpoubled the size of the arrya
		Collectibles** temp = new Collectibles * [tempsize];

		for (int i = 0; i < size; ++i)
		{
			temp[i] = collectibles[i];
		}
		for (int i = size; i < tempsize; ++i)
		{
			temp[i] = nullptr;
		}

		size = tempsize;
		delete[] collectibles;
		collectibles = temp;

	}

public:
	virtual ~CollectibleFactory()
	{
		for (int i = 0;i < current;i++)
			delete collectibles[i];
		delete[] collectibles;
	}
	Collectibles** getCollectibles() 
	{ 
		return collectibles; 
	}
	int getCount() const 
	{ 
		return current; 
	}

	void removeCollectible(int index) 
	{
		if (index < 0 || index >= current) return;
		delete collectibles[index];

		for (int i = index; i < current - 1; i++) 
		{
			collectibles[i] = collectibles[i + 1];
		}

		//set the last position to nullptr and decreased count
		collectibles[current - 1] = nullptr;
		current--;
	}

};