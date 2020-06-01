#pragma once

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

#include "sfVectorUtils.h"
#include "MathUtils.h"

#include <iostream>


class Wall
{
private:

	// Position, velocity and acceleration of centre
	sf::Vector2f pos, posPrev;
	sf::Vector2f vel;
	sf::Vector2f acc;

	// Rectangle representing the wall
	sf::RectangleShape shape;

	// Rotation angle and angular velocity, in radians
	float theta, thetaPrev;
	float omega;

public:
	// Constructors & destructors
	Wall(sf::Vector2f pos, sf::Vector2f vel, sf::Vector2f acc, float length, float thickness, float theta, float omega);
	~Wall();

	// Accessors 
	const sf::FloatRect& getGlobalBounds() const { return shape.getGlobalBounds(); }
	const sf::RectangleShape& getShape() const { return shape; }
	const sf::Vector2f& getPos() const { return pos; }
	const sf::Vector2f& getVel() const { return vel; }
	const sf::Vector2f& getAcc() const { return acc; }
	const float& getTheta() const { return theta; }
	const float& getOmega() const { return omega; }

	// Mutators
	void setAcc(const sf::Vector2f& acc) { this->acc = acc; }

	// Other functions
	void update(const float& dt);
	void draw(sf::RenderTarget* target, float alpha);
};

