#include "TileChunk.h"

TileChunk::TileChunk(void) 
	:hasGenerated{false}, collided{false}, player{nullptr}, tileSize{0}, tileScale{0},
	tileset{ nullptr } {
}
TileChunk::TileChunk(const TileChunk& tchunk) {
	this->tileSize = tchunk.tileSize;
	this->tileScale = tchunk.tileScale;
	this->layout = tchunk.layout;
	this->player = tchunk.player;
	this->tiles = tchunk.tiles;
	this->tileset = tchunk.tileset;
	this->position = tchunk.position;
	this->hasGenerated = tchunk.hasGenerated;
	this->destructionTime = tchunk.destructionTime;
}
TileChunk::~TileChunk(void) {
	for (auto& i : this->tiles) i = nullptr;
	this->tiles.clear();
}
TileChunk& TileChunk::operator=(const TileChunk& tchunk) {
	if (this != &tchunk) {
		this->tileSize = tchunk.tileSize;
		this->tileScale = tchunk.tileScale;
		this->layout = tchunk.layout;
		this->player = tchunk.player;
		this->tiles = tchunk.tiles;
		this->tileset = tchunk.tileset;
		this->position = tchunk.position;
		this->hasGenerated = tchunk.hasGenerated;
		this->destructionTime = tchunk.destructionTime;
	}
	return *this;
}
const sf::Vector2u& TileChunk::GetElementSize(void) const {
	return { static_cast<unsigned>(this->layout.at(0).size()), 
			 static_cast<unsigned>(this->layout.size()) };
}
const sf::Vector2f& TileChunk::GetSize(void) const {
	return { static_cast<float>(this->layout.at(0).size() * this->tileSize * this->tileScale),
			 static_cast<float>(this->layout.size() * this->tileSize * this->tileScale) };
}
void TileChunk::GenerateTiles(const unsigned& tileSize, const unsigned& tileScale) {
	this->tileSize = tileSize;
	this->tileScale = tileScale;

	for (size_t i = 0; i < layout.size(); i++) {
		for (size_t j = 0; j < layout.at(0).size(); j++) {
			unsigned collType = this->layout.at(i).at(j) / 0x64;
			unsigned tileLoc = this->layout.at(i).at(j) % 0x64;

			unsigned x = tileLoc / 0xA;
			unsigned y = tileLoc % 0xA;

			bool isEmpty = (x + y) < 0x1;
			bool collDirs[4] = {true, true, true, true};

			if (i <= layout.size() - 1) {
				if (i != 0 && this->layout.at(i - 1).at(j) != 0xC8
					&& this->layout.at(i - 1).at(j) / 0x64 != 0x2) collDirs[0] = false; //top
				if (i != layout.size() - 1 && this->layout.at(i + 1).at(j) != 0xC8
					&& this->layout.at(i + 1).at(j) / 0x64 != 0x2) collDirs[1] = false; //bottom	
			}
			if (j <= layout.at(0).size() - 1) {
				if (j != 0 && this->layout.at(i).at(j - 1) != 0xC8
					&& this->layout.at(i).at(j - 1) / 0x64 != 0x2) collDirs[2] = false; //left	
				if (j != layout.at(0).size() - 1 && this->layout.at(i).at(j + 1) != 0xC8
					&& this->layout.at(i).at(j + 1) / 0x64 != 0x2) collDirs[3] = false; //right
			}

			using CT = Tile::TileCollType;
			CT tileCollType;

			switch (collType) {
				case 0x1: tileCollType = CT::COLLISION; break;
				case 0x2: tileCollType = CT::DECOR; break;
				case 0x3: tileCollType = CT::DEADLY; break;
				case 0x4: tileCollType = CT::BOUNCE; break;
				case 0x5: tileCollType = CT::SELFD; break;
				default: tileCollType = CT::DECOR; break;
			};

			sf::IntRect intrect(x * this->tileSize, y * this->tileSize, this->tileSize, this->tileSize);
			sf::Vector2f pos(this->position.x + (this->tileSize * j * tileScale), this->position.y + (this->tileSize * i * tileScale));
			
			if (!isEmpty) this->tiles.push_back(new Tile(pos, this->tileset, intrect, tileCollType, tileScale, collDirs));
		}
	}
}
void TileChunk::DrawChunk(sf::RenderTarget& target) {
	for (auto& i : this->tiles) {
		target.draw(i->sprite);
		sf::RectangleShape rs;
		rs.setFillColor(i->collisionType == Tile::TileCollType::COLLISION ? sf::Color(255, 0, 0, 125) 
			: sf::Color(13, 0, 255, 125));
		rs.setPosition(i->collisionRect.position);
		rs.setSize(i->collisionRect.size);
		target.draw(rs);
	}
}
void TileChunk::CheckPlayerCollision() {
	if (this->IsWithinBounds(this->player->collisionRect)) {
		for (auto& tile : this->tiles) {
			using CLD = CollisionRect::CollisionDirection;

			if (tile->collisionType == Tile::TileCollType::COLLISION || tile->collisionType == Tile::TileCollType::BOUNCE
				|| tile->collisionType == Tile::TileCollType::SELFD) {
				if (tile->collisionRect.IsCollidingWith(player->collisionRect)) {
					CLD colDir = player->collisionRect.GetCollisionDirection(tile->collisionRect);

					if (tile->collDirs[0] && player->velocity.y < 0.0f
						|| tile->collDirs[1] && player->velocity.y > 0.0f) {
						if (player->position.x < tile->collisionRect.position.x) colDir = CLD::LEFT;
						else if (player->position.x > tile->collisionRect.position.x +
							tile->collisionRect.size.x - player->collisionRect.size.x) colDir = CLD::RIGHT;
					}

					if (!this->collided) { this->destTimer.restart(); }
					this->collided = true;

					tile->coldir = colDir;
					tile->colliding = true;
					switch (colDir) {
					case CLD::TOP: if (tile->collDirs[0] && player->velocity.y > 0.0f) {
						if (tile->collisionType == Tile::TileCollType::BOUNCE) player->velocity.y = -130.0f;
						else player->isGrounded = true;
					}
						break;
					case CLD::BOTTOM: if (tile->collDirs[1]) player->velocity.y *= -1; break;
					case CLD::LEFT: if (tile->collDirs[2]) player->canMoveRight = false; break;
					case CLD::RIGHT: if (tile->collDirs[3]) player->canMoveLeft = false; break;
					default: break;
					}
				}
				if (tile->colliding && !tile->collisionRect.IsCollidingWith(player->collisionRect)) {
					switch (tile->coldir) {
					case CLD::TOP: player->isGrounded = false; break;
					case CLD::BOTTOM: break;
					case CLD::LEFT: player->canMoveRight = true; 
						if (player->position.y < tile->collisionRect.position.y)
							player->isGrounded = false; break;
					case CLD::RIGHT: player->canMoveLeft = true; 
						if (player->position.y < tile->collisionRect.position.y) 
							player->isGrounded = false; break;
					default: break;
					}
					tile->coldir = CLD::NONE;
					tile->colliding = false;
				}
			}
			else if (tile->collisionType == Tile::TileCollType::DEADLY && tile->collisionRect.IsCollidingWith(player->collisionRect)) {
				//printf("ded\n");
			}
			else continue;
		}
	}
}
void TileChunk::UpdatePosition(void) 
{
	std::vector<sf::Vector2f> vec;

	for (size_t i = 0; i < layout.size(); i++) {
		for (size_t j = 0; j < layout.at(0).size(); j++) {
			sf::Vector2f pos(this->position.x + (this->tileSize * j * tileScale), this->position.y + (this->tileSize * i * tileScale));
			
			unsigned tileLoc = this->layout.at(i).at(j) % 0x64;

			unsigned x = tileLoc / 0xA;
			unsigned y = tileLoc % 0xA;

			bool isEmpty = (x + y) < 0x1;
			
			if (!isEmpty) vec.push_back(pos);
		}
	}

	for (size_t i = 0; i < vec.size(); i++) {
		this->tiles.at(i)->SetPosition(vec.at(i)); 
		this->tiles.at(i)->collisionRect.size = { this->tiles.at(i)->collisionRect.size.x * this->tileScale, 
												  this->tiles.at(i)->collisionRect.size.y * this->tileScale };
		this->tiles.at(i)->collisionRect.position = vec.at(i);
	}
	
	vec.clear();
}
void TileChunk::Update(const float& dt){
	if (this->destTimer.getElapsedTime().asSeconds() > this->destructionTime && collided) {
		//this->position.y += 50.5f * dt;
		this->UpdatePosition();
	}
}
bool TileChunk::IsWithinBounds(CollisionRect& crect) const {
	return (crect.position.x + crect.size.x) > this->position.x || crect.position.x < (this->position.x + this->GetSize().x);
}
bool TileChunk::IsWithinView(const sf::View& view) const {
	return (this->position.x + this->GetSize().x) > (view.getCenter().x - view.getSize().x / 2);
}