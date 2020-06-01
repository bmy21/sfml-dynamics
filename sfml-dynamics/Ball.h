#pragma once

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

#include "sfVectorUtils.h"
#include "Wall.h"

#include <iostream>
#include <tuple>

class Ball
{
private:
	// Position, velocity and acceleration
	sf::Vector2f pos, posPrev;
	sf::Vector2f vel;
	sf::Vector2f acc;

	// Mass
	float mass;

	// Circle represting the ball
	float radius;
	sf::CircleShape shape;
	

public:
	// Constructors & destructors
	Ball(sf::Vector2f pos, sf::Vector2f vel, sf::Vector2f acc, float mass, float radius, sf::Color col);
	~Ball();

	// Accessors
	const sf::Vector2f& getPos() const { return pos; }
	const sf::Vector2f& getVel() const { return vel; }
	const sf::Vector2f& getAcc() const { return acc; }
	const float& getMass() const { return mass; }
	const float& getRadius() const { return radius; }


	// Mutators
	void setPos(sf::Vector2f pos) { this->pos = pos; }
	void setVel(sf::Vector2f vel) { this->vel = vel; }
	void setAcc(sf::Vector2f acc) { this->acc = acc; }

	// Other functions
	bool checkForCollision(const Ball& q);
	void rigidBodyCollide(Ball& q);

	void confine(const sf::FloatRect& bounds);

	bool isInside(const Wall& w);
	std::tuple<bool, sf::Vector2f, sf::Vector2f, float> checkForCollision(const Wall& w);
	void rigidBodyCollide(const Wall& w, const sf::Vector2f& contactPoint, const sf::Vector2f& collisionNormal, float overlap);
	
	void move(const float& dt);
	void draw(sf::RenderTarget* target, float alpha);
};

