#include "Game.h"

Game::Game(void) 
{

}
Game::~Game()
{

}

sf::RenderWindow window(sf::VideoMode(0x3E8, 0x3E8), "SFML Game", sf::Style::Default);
sf::Event _event;

sf::Vector2f Center(15, 500);
sf::Vector2f HalfSize(200, 200);
sf::View View1(Center, HalfSize);

Player player;
Map map("Res/Tileset/caves.png");

void Game::Init(void)
{
	window.setView(View1);

	player.Init();
	map.Init(&player);
}
void Game::Events(void)
{
	while (window.pollEvent(_event))
	{
		player.Events(_event);
		if (_event.type == sf::Event::Closed)
			window.close();
		if (_event.type == sf::Event::KeyReleased)
		{
			if (_event.key.code == sf::Keyboard::Escape)
				window.close();
		}
	}
}
void Game::UpdateDeltaTime(void)
{
	this->deltaTime = this->deltaTimeClock.restart().asSeconds();
}
void Game::Update(void)
{
	this->UpdateDeltaTime();
	player.Update(this->deltaTime);
	map.Update();
}
void Game::Draw(void)
{
	player.Draw(window);
	map.Draw(window);
}
void Game::Run(void)
{
	this->Init();
	while (window.isOpen()) 
	{
		this->Events();
		this->Update();
		window.clear();
		this->Draw();
		window.display();
	}
}