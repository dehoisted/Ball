#include <SFML/Graphics.hpp>
#include <iostream>
#include <Windows.h>
#include <thread>
#include "Game.h"
#include "Music.hpp"
#include "Move.hpp"

int main()
{
#ifdef _DEBUG
	std::cout << "Debug mode ON.\n";
#endif

#if BALL_SHOW_CONSOLE_WINDOW == 0
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#elif BALL_SHOW_CONSOLE_WINDOW == 1
	std::cout << "BALL_SHOW_CONSOLE_WINDOW was 1/TRUE.\n";
#else
#error BALL_SHOW_CONSOLE_WINDOW must be either have the value of 0 or 1.
#endif

	sf::RenderWindow window(sf::VideoMode(windowMode.first, windowMode.second), windowName, sf::Style::Titlebar | sf::Style::Close);
	window.setFramerateLimit(framerateLimit);

	/* Font */
	sf::Font globalFont;
	if (!globalFont.loadFromFile(fontFile))
		return EXIT_FAILURE;

	/* Logo */
	sf::Texture ballLogoTexture;
	if (!ballLogoTexture.loadFromFile(ballLogoFile))
		return EXIT_FAILURE;
	sf::Sprite ballLogo(ballLogoTexture);
	ballLogo.setPosition(150, 0);

	/* Texts */
	sf::Text menu;
	menu.setFont(globalFont);
	menu.setCharacterSize(30);
	menu.setPosition(148.f, 175.f);
	menu.setFillColor(sf::Color::White);
	menu.setString(menuMsg);

	sf::Text scoreInfo;
	scoreInfo.setFont(globalFont);
	scoreInfo.setCharacterSize(20);
	scoreInfo.setPosition(0, 0);
	scoreInfo.setFillColor(sf::Color::White);

	/* Client */
	sf::CircleShape clientShape;
	clientShape.setRadius(50.0);
	sf::Texture client_texture;
	if (!client_texture.loadFromFile(ballTextureFile))
		return EXIT_FAILURE;
	clientShape.setTexture(&client_texture);
	clientShape.setFillColor(sf::Color::White);
	clientShape.setPosition(300, 0);

	/* Target */
	sf::CircleShape targetShape;
	targetShape.setRadius(clientShape.getRadius() / static_cast<float>(6));
	targetShape.setFillColor(sf::Color::Red);
	targetShape.setOutlineThickness(2);
	targetShape.setOutlineColor(sf::Color::White);
	targetShape.setPosition(300, 300);

	/* Sound */
	sf::SoundBuffer collisionSoundBuffer;
	if (!collisionSoundBuffer.loadFromFile(collisionFile))
		return EXIT_FAILURE;
	sf::Sound collisionSound(collisionSoundBuffer);
	
	/* Program Loop */
	while (window.isOpen() && !sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		static auto gameStatus = GameStatus::Menu;
		static sf::Clock clock; static sf::Time endTime = sf::seconds(10.0f);
		static unsigned int score = 0, highScore = 0;
		static Move move{};
		sf::Event event{};
		
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			/* Basic Keybinds */
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
				switch (gameStatus)
				{
				case GameStatus::Menu:
					gameStatus = GameStatus::Playing;
					break;
				case GameStatus::Playing:
					menu.setString(pauseMsg);
					gameStatus = GameStatus::Paused;
					break;
				case GameStatus::Error:
					window.close();
					break;
				default: // Paused & Lost
					gameStatus = GameStatus::Playing;
					break;
				}
				sf::sleep(sf::milliseconds(125));
			}

#if BALL_CONTROL == 0x02 // WASD
			// Y
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				clientShape.move(0, globalStep * (-1)); // Up
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				clientShape.move(0, globalStep); // Down
			// X
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				clientShape.move(globalStep * (-1), 0); // Left
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				clientShape.move(globalStep, 0); // Right
#elif BALL_CONTROL == 0x04 // ArrowKeys
			// Y
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				clientShape.move(0, globalStep * (-1)); // Up
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				clientShape.move(0, globalStep); // Down
			// X
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				clientShape.move(globalStep * (-1), 0); // Left
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				clientShape.move(globalStep, 0); // Right
#else
#error BALL_CONTROL enumeration value not found.
#endif
			// Switch Ball Texture
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))
			{
				static std::vector<sf::Color> clientColors = {
					sf::Color::Blue, sf::Color::Magenta, sf::Color::Yellow,
					sf::Color::Cyan, sf::Color::Green,sf::Color::White
				};
				static ptrdiff_t index = 0;
				auto color = std::next(clientColors.begin(), index);
				clientShape.setFillColor(*color);
				index++;
				if (index == static_cast<int>(clientColors.size()))
					index = 0;
				std::cout << "Changed client ball color\n";
				sf::sleep(sf::milliseconds(25));
			}

			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			{
				musicRestart = musicRestart ? false : true;
				sf::sleep(sf::milliseconds(100));
			}

			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
				music = music ? false : true;

				if (music) {
					std::thread song(playMusic, musicFile);
					song.detach();
				}
				sf::sleep(sf::milliseconds(100));
			}
		}

		bounceSetAndCheck(clientShape, move);
		bool collided = collisionCheckAndSet(clientShape, targetShape, collisionSound);
		boundsCheckAndSet(clientShape, &move);
		//std::cout << "client\tX coord: " << clientShape.getPosition().x << "\tY coord: " << clientShape.getPosition().y << '\n'; // Log

		if (collided)
		{
			score++;
			highScore = score > highScore ? score : highScore;
		}

		scoreInfo.setString("High Score: "
			+ std::to_string(highScore) + "\nCurrent Score: "
			+ std::to_string(score) + "\nTime: "
			+ std::to_string(clock.getElapsedTime().asSeconds()) + " s");

		if (clock.getElapsedTime() > endTime)
		{
			static unsigned int validScore = 0;
			validScore += 5;
			if (score < validScore)
			{
				score = 0;
				validScore = 0;
				menu.setString(lostMsg);
				gameStatus = GameStatus::Lost;
				endTime = sf::seconds(10.0f);
			}

			else endTime += sf::seconds(10.0f);
		}
		
		window.clear();
		if (gameStatus == GameStatus::Playing)
		{
			window.draw(clientShape);
			window.draw(targetShape);
			window.draw(scoreInfo);
		}
		else
		{
			window.draw(ballLogo);
			window.draw(menu);
			clock.restart();
		}
		window.display();
	}
}
