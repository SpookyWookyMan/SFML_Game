#pragma once

#include <SFML/Graphics.hpp>
#include "../Entities/Player.h"
#include <iostream>

struct Layer 
{
	Layer(const char* _path, sf::View& view) : deviation{ 0.0f }, mod{ 1 } {
		txtr.loadFromFile(_path);
		spr.setTexture(txtr);
		txtr.setRepeated(true);

		scale = { view.getSize().y / txtr.getSize().y ,
				  view.getSize().y / txtr.getSize().y };

		sf::Vector2f pos(view.getCenter().x - view.getSize().x, view.getCenter().y + view.getSize().y / 2);
		sf::Vector2f sprdim(txtr.getSize().x* scale.x, txtr.getSize().y* scale.y);

		spr.setOrigin({0.0f, sprdim.y});
		spr.setPosition(pos);
		spr.setTextureRect({ 0, 0, static_cast<int>(txtr.getSize().x),
							       static_cast<int>(txtr.getSize().y) });
		spr.setScale(scale);
	}

	sf::Vector2f scale;

	sf::Sprite spr;
	sf::Texture txtr;

	float deviation;

	unsigned mod;
};

class Parallax {
public:

	std::vector<Layer> layers;

	sf::Sprite sprite;//test
	sf::Texture t;
	
	Player player;

	float deviation = 0.0f;
	float speed = 0.5f;
	unsigned mod = 1;

	bool canMove;

	void Init(Player& player, sf::View& view);
	void Update(const float& dt, sf::View& view);
	void Draw(sf::RenderTarget& target);

private:

	//void AddLayer(const char*);

};