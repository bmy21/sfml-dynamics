#include "Sim.h"

Sim::Sim()
	: window(nullptr), frameTime(0.f), accumulatedTime(0.f), dtPhysics(1.f / 400.f)
{
	randomEngine.seed(std::chrono::system_clock::now().time_since_epoch().count());

	initWindow();
	initWalls();
	initBalls();
}

Sim::~Sim()
{
	// Free up allocated memory
	delete window;

	for (auto b : balls)
		delete b;

	for (auto w : walls)
		delete w;
}

void Sim::initWindow()
{
	// Create the window
	sf::ContextSettings windowSettings;
	windowSettings.antialiasingLevel = 4;
	window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "SFML Dynamics", sf::Style::Titlebar | sf::Style::Close, windowSettings);
	

	window->setVerticalSyncEnabled(0); // Vsync on or off?
}

void Sim::initBalls()
{
	// Distributions to draw particle parameters from
	std::uniform_real_distribution<float> xDist(0, window->getSize().x);
	std::uniform_real_distribution<float> yDist(0, window->getSize().y);
	std::uniform_real_distribution<float> vDist(-200, 200);
	std::uniform_real_distribution<float> massDist(5, 5);
	std::uniform_real_distribution<float> radiusDist(18, 30);
	std::uniform_real_distribution<float> colDist(0, 255);
	
	// Populate the particle vector
	for (int i = 0; i < 200; ++i)
	{
		Ball* b = new Ball(sf::Vector2f(xDist(randomEngine), yDist(randomEngine)),
			sf::Vector2f(vDist(randomEngine), vDist(randomEngine)),
			sf::Vector2f(0, 300),
			massDist(randomEngine),
			radiusDist(randomEngine),
			sf::Color(colDist(randomEngine), colDist(randomEngine), colDist(randomEngine), 255));

		// Make sure there are no collisions at t = 0
		for (auto w : walls)
			for (auto c : balls)
			{
				do
				{
					b->setPos(sf::Vector2f(xDist(randomEngine), yDist(randomEngine)));
					c->setPos(sf::Vector2f(xDist(randomEngine), yDist(randomEngine)));
				} while (b->isInside(*w) || c->isInside(*w));
			}

		balls.push_back(b);
	}


	balls.push_back(new Ball(sf::Vector2f(400, 450),
		sf::Vector2f(100, 010),
		sf::Vector2f(0, 250),
		10,
		20,
		sf::Color::Cyan));
}

void Sim::initWalls()
{

	Wall* w = new Wall(sf::Vector2f(window->getSize().x * 1.f / 4.f, window->getSize().y / 2.f + 80.f),
		sf::Vector2f(0.f, 0.f),
		sf::Vector2f(0.f, 0.f), 
		15, 900, 90.f*pi/180.f, pi/4.f);
	walls.push_back(w);
}

void Sim::run()
{
	while (window->isOpen())
	{
		updateFrameTimer();
		handleEvents();
		updateLogic();
		render();
	}
}

void Sim::updateFrameTimer()
{

	// Update frame timer
	frameTime = frameTimer.restart().asSeconds();


	// Limit frame time (corresponding to 15FPS) to prevent clock running while
	// logic is paused (when moving window)
	if (frameTime > 1.f / 15.f)
		frameTime = 1.f / 15.f;

	// Need to process as much extra time as elapsed during the frame
	accumulatedTime += frameTime;


	// std::cout << 1.f / frameTime << '\n';
}

void Sim::handleEvents()
{
	// Handle SFML events
	sf::Event event;
	while (window->pollEvent(event))
	{
		// Should we close the window?
		if (event.type == sf::Event::Closed)
			window->close();
	}
}

void Sim::updateLogic()
{
	// Keep processing while we still have time to process
	while (accumulatedTime > dtPhysics)
	{

		// Apply a spring force to the wall
		walls[0]->setAcc(sf::Vector2f(-0.5f * (walls[0]->getPos().x - window->getSize().x / 2.f), 0.f));

		// Move the particles 
		for (auto b : balls)
			b->move(dtPhysics);

		// Move the walls
		for (auto w : walls)
			w->update(dtPhysics);


		// Implement collisions, first wall-ball, then ball-ball
		for (auto b : balls)
			for (auto w : walls)
			{
				// Did ball b collide with wall w?
				auto collisionParams = b->checkForCollision(*w);
				if (std::get<0>(collisionParams))
				{
					// Collide as a rigid body (need to pass in collision normal and amount of overlap)
					b->rigidBodyCollide(*w, std::get<1>(collisionParams), std::get<2>(collisionParams), std::get<3>(collisionParams));
				}
			}
		
		collideBalls();

		// We processed an amount dtPhysics of time
		accumulatedTime -= dtPhysics;
	}
}

void Sim::render()
{
	// Fraction of time left over
	float alpha = accumulatedTime / dtPhysics;

	// Erase previous frame
	window->clear(sf::Color::White);

	// Draw objects
	for (auto w : walls)
		w->draw(window, alpha);

	for (auto b : balls)
		b->draw(window, alpha);

	window->display();
}

void Sim::collideBalls()
{
	// Process particle-particle collisions

	// Loop over all particles in the vector
	for (int i = 0; i < balls.size(); ++i)
	{
		Ball* p = balls[i];

		for (int j = 0; j < i; ++j)
		{
			Ball* q = balls[j];

			if (p->checkForCollision(*q))
			{
				p->rigidBodyCollide(*q);
			}
		}

		sf::FloatRect bounds(0, 0, window->getSize().x, window->getSize().y);
		p->confine(bounds);
	}
}