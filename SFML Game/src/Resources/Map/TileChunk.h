#pragma once

#include <iostream>
#include "Tile.h"
#include "../../Collision/CollisionRect.h"
#include "../../Entities/Player.h"

class TileChunk 
{
public:

	unsigned tileSize;

	Player* player;

	sf::Texture* tileset;
	sf::Vector2f position;

	std::vector<std::vector<unsigned> > layout;
	std::vector<Tile*> tiles;

	TileChunk(void);
	~TileChunk(void);

	const sf::Vector2u& GetSize(void) const;
	void GenerateTiles();
	void DrawChunk(sf::RenderTarget& target);
	void CheckPlayerCollision(void);
};