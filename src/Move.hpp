#pragma once
#include <random>

constexpr float globalMove = 20.0f;
constexpr float globalStep = globalMove / 2; // 10.0f

enum class Move
{
	up,
	down,
	left,
	right
};

static inline bool moveWithinRange(const float input1, const float input2, const float deviation)
{
	return std::fabsf(input1 - input2) <= deviation;
}

static inline unsigned int genRandomMove(const char coord)
{
	std::random_device dev;
	std::mt19937 rng(dev());
	unsigned int max = 0;

	switch (coord)
	{
	case 'x':
		max = 600;
		break;
	case 'y':
		max = 400;
		break;
	default:;
	}
	return std::uniform_int_distribution<std::mt19937::result_type>(0, max)(rng);
}

/* Bounce Direction Checking & Setting */
static inline void bounceSetAndCheck(sf::CircleShape& clientShape, const Move& move)
{
	switch (move)
	{
	case Move::up:
		clientShape.move(0, globalMove * (-1));
		break;
	case Move::down:
		clientShape.move(0, globalMove);
		break;
	case Move::left:
		clientShape.move(globalMove * (-1), 0);
		break;
	case Move::right:
		clientShape.move(globalMove, 0);
		break;
	}
}

/* Colision/Target Checking */
static inline bool collisionCheckAndSet(const sf::CircleShape& clientShape,
	sf::CircleShape& targetShape, sf::Sound& ballSound)
{
	if (moveWithinRange(clientShape.getPosition().x, targetShape.getPosition().x, 10) && moveWithinRange(clientShape.getPosition().y, targetShape.getPosition().y, 10))
	{
		std::cout << "ball colision has been detected\n";
		std::cout << "target\tX coord: " << targetShape.getPosition().x << "\tY coord: " << targetShape.getPosition().y << '\n';
		ballSound.play();
		const auto xrand = static_cast<float>(genRandomMove('x'));
		const auto yrand = static_cast<float>(genRandomMove('y'));
		targetShape.setPosition(xrand, yrand);
		std::cout << "moved target shape to random spot.\n";
		return true;
	}
	return false;
}

/* Bounds Checking */
static inline void boundsCheckAndSet(sf::CircleShape& clientShape, Move* move)
{
	// X
	if (clientShape.getPosition().x < 0) {
		clientShape.setPosition(0, clientShape.getPosition().y);
		*move = Move::right;
	}

	else if (clientShape.getPosition().x > 600) {
		clientShape.setPosition(600, clientShape.getPosition().y);
		*move = Move::left;
	}

	// Y 
	else if (clientShape.getPosition().y < 0) {
		clientShape.setPosition(clientShape.getPosition().x, 0);
		*move = Move::down;
	}

	if (clientShape.getPosition().y > 400) {
		clientShape.setPosition(clientShape.getPosition().x, 400);
		*move = Move::up;
	}
}