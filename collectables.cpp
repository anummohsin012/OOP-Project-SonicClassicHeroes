#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;

class Collectibles {
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
	 SoundBuffer ringSound;
	 bool ringSoundLoaded;
public:
	Rings(int r,int c, char** lvl):Collectibles(r,c)
	{
		ringSoundLoaded = false;
		texture.loadFromFile("Data/ring.png");
		sprite.setTexture(texture);
		sprite.setScale(1.5f, 1.5f);

		lvl[r][c] = 'R';
	}
	virtual void collect(char** lvl)
	{
		lvl[row][column] = ' ';
	}
	virtual void playSound() 
	{
		if (!ringSoundLoaded) 
		{
			ringSound.loadFromFile("Data/Ring.wav");
			ringSoundLoaded = true;
		}
		Sound sound(ringSound);
		sound.play();
	}
	~ Rings(){}
	virtual string typeOfCollectible()
	{
		return "ring";
	}
};
//SoundBuffer Rings::ringSound;
//bool Rings::ringSoundLoaded = false;


class ExtraLives: public Collectibles {
	 SoundBuffer lifeSound;
	bool lifeSoundLoaded;
public:
	ExtraLives(int r, int c, char** lvl) : Collectibles(r, c)
	{
		texture.loadFromFile("Data/life.png");
		sprite.setTexture(texture);
		sprite.setScale(3.0f, 3.0f);
		lifeSoundLoaded = false;
		lvl[r][c] = 'L';
	}
	
	virtual void collect(char** lvl)
	{
		lvl[row][column] = ' ';
	}
	~ ExtraLives(){}
	virtual void playSound() {
		if (!lifeSoundLoaded) {
			lifeSound.loadFromFile("Sprites\Sonic the Hedgehog CD 2011 - Sound Effects\Stage\LargeBooster.wav");
			lifeSoundLoaded = true;
		}
		Sound sound(lifeSound);
		sound.play();
	}
	virtual string typeOfCollectible()
	{
		return "extralife";
	}
};
//SoundBuffer ExtraLives::lifeSound;
//bool ExtraLives::lifeSoundLoaded = false;

class SpecialAbility: public Collectibles {
SoundBuffer abilitySound;
	 bool abilitySoundLoaded;
public:
	SpecialAbility(int r, int c, char** lvl) : Collectibles(r, c)
	{
		texture.loadFromFile("Data/bonus.png");
		sprite.setTexture(texture);
		sprite.setScale(3.0f,3.0f);
		lvl[r][c] = 'A';
		abilitySoundLoaded = false;
	}
	
	virtual void collect(char** lvl)
	{
		lvl[row][column] = ' ';
	}
	virtual void playSound() {
		if (!abilitySoundLoaded) {
			abilitySound.loadFromFile("Sprites\Sonic the Hedgehog CD 2011 - Sound Effects\Global\SpecialRing.wav");
			abilitySoundLoaded = true;
		}
		Sound sound(abilitySound);
		sound.play();
	}
	~SpecialAbility(){}
	virtual string typeOfCollectible()
	{
		return "specialability";
	}
};
//SoundBuffer SpecialAbility::abilitySound;
//bool SpecialAbility::abilitySoundLoaded = false;

class CollectibleFactory {
	int size;
	Collectibles** collectibles;
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



	void removeCollectible(int index) {
		if (index < 0 || index >= current) return;

		// Delete the object
		delete collectibles[index];

		// Shift remaining elements to fill the gap
		for (int i = index; i < current - 1; i++) {
			collectibles[i] = collectibles[i + 1];
		}

		// Set the last position to nullptr and decrease count
		collectibles[current - 1] = nullptr;
		current--;
	}

};