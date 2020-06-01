#include "Wall.h"


Wall::Wall(sf::Vector2f pos, sf::Vector2f vel, sf::Vector2f acc, float length, float thickness, float theta, float omega)
	: pos(pos), posPrev(pos), vel(vel), acc(acc), theta(theta), thetaPrev(theta), omega(omega)
{
	shape.setPosition(pos);
	shape.setOrigin(sf::Vector2f(length / 2.f, thickness / 2.f));
	shape.setSize(sf::Vector2f(length, thickness));
	shape.setRotation(theta);
	shape.setFillColor(sf::Color::Black);
}


Wall::~Wall()
{

}

void Wall::update(const float& dt)
{
	// Save previous rotation angle and position for interpolation
	thetaPrev = theta;
	posPrev = pos;


	// Rotate according to angular velocity,
	// then constrain to 0-360 degrees
	theta = wrap(theta + omega * dt);

	// Integrate linear motion
	pos += vel * dt + 0.5f * acc * dt * dt;
	vel += acc * dt;

	shape.setRotation(theta * 180.f / pi);
	shape.setPosition(pos);
}

void Wall::draw(sf::RenderTarget* target, float alpha)
{	
	sf::RectangleShape shapeToDraw(shape);

	// Interpolate the position
	shapeToDraw.setPosition(pos * alpha + posPrev * (1.f - alpha));

	// Interpolate the orientation vector, and use this to get the interpolated angle
	float angle1 = thetaPrev, angle2 = theta;
	sf::Vector2f normal(cosf(theta), sinf(theta)), normalPrev(cosf(thetaPrev), sinf(thetaPrev));
	sf::Vector2f interpolatedNormal = normal * alpha + normalPrev * (1.f - alpha);
	float interpolatedAngle = wrap(atan2f(interpolatedNormal.y, interpolatedNormal.x));

	shapeToDraw.setRotation(interpolatedAngle * 180.f / pi);
	target->draw(shapeToDraw);
}
