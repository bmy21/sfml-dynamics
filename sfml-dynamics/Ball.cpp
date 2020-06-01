#include "Ball.h"

Ball::Ball(sf::Vector2f pos, sf::Vector2f vel, sf::Vector2f acc, float mass, float radius, sf::Color col)
	: pos(pos), vel(vel), acc(acc),
	posPrev(pos),
	radius(radius), mass(mass)
{
	shape.setRadius(radius); 
	shape.setOrigin(radius, radius);
	shape.setFillColor(col);
}

Ball::~Ball()
{

}

// Has this ball collided with ball q?
bool Ball::checkForCollision(const Ball& q)
{
	// Collision if distance between centres less than sum of radii
	if (vectorMag(q.getPos() - pos) < radius + q.getRadius())
		return true;

	return false;
}

// Bounce off ball q as a rigid body
// Modifies both p and q
void Ball::rigidBodyCollide(Ball& q)
{
	// Coefficient of restitution
	float e = 0.6f;

	// Coefficient of friction
	float mu = 0.05f;

	// Retrieve positions and velocities of the balls
	sf::Vector2f rp = pos, rq = q.getPos(),
		vp = vel, vq = q.getVel();
	
	// Retrieve masses and radii
	float mp = mass, mq = q.getMass(), Rp = radius, Rq = q.getRadius();

	// Collision normal from q->p
	sf::Vector2f normal = normalise(rp-rq);

	// First set normal component of velocity to zero, then reassign according to conservation of momentum
	vel = vp
		- normal * dotProduct(vp, normal) 
		+ normal * (dotProduct(vp, normal) * (mp - e * mq) + dotProduct(vq, normal) * mq * (1.f + e)) / (mp + mq); 

	q.setVel(vq
		- normal * dotProduct(vq, normal)
		+ normal * (dotProduct(vq, normal) * (mq - e * mp) + dotProduct(vp, normal) * mp * (1.f + e)) / (mp + mq));
	
	

	// Add the frictional impulse
	sf::Vector2f collisionTangent(0.f, 0.f);
	if (vectorMag(vp - vq - dotProduct(vp-vq, normal) * normal) > 0.f)
		collisionTangent = 1.f * normalise(vp - vq - dotProduct(vp - vq, normal) * normal);

	float normalImpulseMagP = vectorMag(-normal * dotProduct(vp, normal)
		+ normal * (dotProduct(vp, normal) * (mp - e * mq) + dotProduct(vq, normal) * mq * (1.f + e)) / (mp + mq));
	vel -= mu * normalImpulseMagP * collisionTangent;

	float normalImpulseMagQ = vectorMag(-normal * dotProduct(vq, normal)
		+ normal * (dotProduct(vq, normal) * (mq - e * mp) + dotProduct(vp, normal) * mp * (1.f + e)) / (mp + mq));
	q.setVel(q.getVel() + mu * normalImpulseMagQ * collisionTangent);


	// Make sure they don't intersect, by moving them apart so that they just touch
	pos = rp + normalise(rp - rq) * 0.5f * ((Rp + Rq) - vectorMag(rq - rp));
	q.setPos(rq + normalise(rq - rp) * 0.5f * ((Rp + Rq) - vectorMag(rq - rp)));
}


void Ball::confine(const sf::FloatRect& bounds)
{
	// Coefficient of restitution
	float e = 0.6f;

	// Confine ball to bounds
	if (pos.x + radius > bounds.left + bounds.width)
	{
		vel.x *= -e;
		pos.x = bounds.left + bounds.width - radius;
	}
	else if (pos.x - radius < bounds.left)
	{
		vel.x *= -e;
		pos.x = bounds.left + radius;
	}

	if (pos.y + radius > bounds.top + bounds.height)
	{
		vel.y *= -e;
		pos.y = bounds.top + bounds.height - radius;
	}
	else if (pos.y - radius < bounds.top)
	{
		vel.y *= -e;
		pos.y = bounds.top + radius;
	}
}


// Check if ball in inside wall (useful for random initialisation)
bool Ball::isInside(const Wall& w)
{
	return w.getGlobalBounds().contains(pos);
}


// This function returns a bool indicating whether a collision is occurring,
// a vector giving the collision normal if so, a vector giving the position of the
// contact point, and the amount by which the two overlap
std::tuple<bool, sf::Vector2f, sf::Vector2f, float>  Ball::checkForCollision(const Wall& w)
{
	// Find the closest point on the rectangle surface and check
	// whether it's less than the radius

	// First get a vector of the wall vertices
	// Note npts is always 4 for a rectangle, but might want to extend later
	int npts = w.getShape().getPointCount();
	std::vector<sf::Vector2f> vertices;
	for (int i = 0; i < npts; ++i)
		vertices.push_back(w.getShape().getTransform().transformPoint(w.getShape().getPoint(i)));
	
	sf::Vector2f closestPoint(0.f, 0.f);
	float closestDistance = 0.f;

	// Now find the closest point on the surface
	for (int i = 0; i < npts; ++i)
	{
		// Construct a vector aligned with side i
		int index1 = 0, index2 = 0;

		if (i < npts - 1)
		{
			index1 = i;
			index2 = i + 1;
		}
		else
		{
			index1 = i;
			index2 = 0;
		}

		const sf::Vector2f& v1 = vertices[index1], v2 = vertices[index2];
		sf::Vector2f edge = normalise(v2 - v1); // Unit vector along this edge
		float lambda = dotProduct(pos - v1, edge); // Distance along this edge

		// Constrain the projection to lie along the shape edge
		if (lambda < 0) lambda = 0;
		else if (lambda > vectorMag(v2 - v1)) lambda = vectorMag(v2 - v1);

		// Projection along the edge in vector form
		sf::Vector2f point = v1 + lambda * edge;
		float distance = vectorMag(pos - point);

		// Initialise the closest distance to the closest distance to edge 0
		if (i == 0) { closestDistance = distance; closestPoint = point; }
		else if (distance < closestDistance) { closestDistance = distance; closestPoint = point; }
	}

	if (closestDistance < radius)
		return std::tuple<bool, sf::Vector2f, sf::Vector2f, float>(true, closestPoint, normalise(pos - closestPoint), radius - closestDistance);

	return std::tuple<bool, sf::Vector2f, sf::Vector2f, float>(false, sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f), 0.f);
}



void Ball::rigidBodyCollide(const Wall& w, const sf::Vector2f& contactPoint, const sf::Vector2f& collisionNormal, float overlap)
{
	// Coefficient of restitution
	float e = 0.6f; 

	// Coefficient of friction
	float mu = 0.4f;

	// Vector from wall centre to contact point
	sf::Vector2f r = contactPoint - w.getPos();

	// Velocity of contact point (using v = w x r, then adding the CoM velocity)
	sf::Vector2f contactVel(-r.y * w.getOmega() + w.getVel().x, r.x * w.getOmega() + w.getVel().y);
	
	// Normal vector along contact surface in direction of incoming velocity
	// Undefined if it comes in with no tangential velocity, so leave as zero vector
	sf::Vector2f collisionTangent(0.f, 0.f);
	
	if (vectorMag(vel - contactVel - dotProduct(vel - contactVel, collisionNormal) * collisionNormal) > 0.f)
		collisionTangent = 1.f* normalise(vel - contactVel - dotProduct(vel - contactVel, collisionNormal) * collisionNormal);


	// First set normal component of velocity to zero, then reassign according to 
	// conservation of momentum (with an infinite mass particle at the collision point)
	sf::Vector2f newVel = vel
		- dotProduct(vel, collisionNormal) * collisionNormal
		+ (1.f + e) * dotProduct(contactVel, collisionNormal) * collisionNormal - e * dotProduct(vel, collisionNormal) * collisionNormal;


	// Adjust tangential velocity for friction
	float normalImpulseMag = vectorMag(-dotProduct(vel, collisionNormal) * collisionNormal + (1.f + e) * dotProduct(contactVel, collisionNormal) * collisionNormal 
		- e * dotProduct(vel, collisionNormal) * collisionNormal);

	newVel -= mu * normalImpulseMag * collisionTangent;

	vel = newVel;

	// Make sure we don't let friction reverse the tangential velocity in a single timestep
	if (dotProduct(newVel - contactVel, collisionTangent) < 0.f) vel -= dotProduct(newVel - contactVel, collisionTangent) * collisionTangent;

	pos += collisionNormal * overlap;
}


// Move the ball
void Ball::move(const float& dt)
{
	// Store previous position (for interpolation)
	posPrev = pos;

	// Integrate the particle's motion
	pos += vel * dt + 0.5f*acc*dt*dt;
	vel += acc * dt;
	
	shape.setPosition(pos);
}

// Render the ball to the target
void Ball::draw(sf::RenderTarget* target, float alpha)
{
	sf::CircleShape shapeToDraw(shape);
	
	// Interpolate the position appropriately
	shapeToDraw.setPosition(pos * alpha + posPrev * (1.f - alpha));
	target->draw(shapeToDraw);
}
