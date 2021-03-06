#include "Tile.h"
#include <iostream>

Tile::Tile(const sf::Vector2f& position, sf::Texture* tileset, const sf::IntRect& txtrRect, const TileCollType& collType, 
	const unsigned& tileScale, bool collDirs[]) {
	this->sprite.setTexture(*tileset);
	this->SetTextureRect(txtrRect);
	this->sprite.setScale({ static_cast<float>(tileScale), 
					        static_cast<float>(tileScale)});
	this->SetPosition(position);
	this->tileScale = tileScale;

	for (size_t i = 0; i < 4; i++) this->collDirs[i] = collDirs[i];

	this->collisionType  = collType;
	this->UpdateCollisionBox();	 
}
Tile::~Tile(void) {
}

void Tile::SetPosition(const sf::Vector2f& position) {
	this->position = position;
	this->UpdateCollisionBox();
}
void Tile::SetTextureRect(const sf::IntRect& rect) {
	this->sprite.setTextureRect(rect);
}
void Tile::UpdateCollisionBox(void) {
	//sf::Vector2f* size = new sf::Vector2f(this->sprite.getTextureRect().width * this->tileScale,
	//					   				  this->sprite.getTextureRect().height * this->tileScale);

	sf::Vector2f* size = new sf::Vector2f(this->sprite.getTexture()->getSize().x * this->sprite.getScale().x,
										  this->sprite.getTexture()->getSize().y * this->sprite.getScale().y);

	this->sprite.setPosition(this->position);
	this->collisionRect.size = *size;
	this->collisionRect.position = this->position;

	delete size;
}