#pragma once

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

#include "Ball.h"
#include "Wall.h"

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <string>

class Sim
{
private:
	// The window to display things in
	sf::RenderWindow* window;

	// Frame timing clock & frame time
	sf::Clock frameTimer;
	float frameTime;

	// Total time that we need to integrate over to keep up with rendering
	float accumulatedTime;

	// Physics timestep (fixed)
	const float dtPhysics; 

	// Used to get random numbers
	std::default_random_engine randomEngine;

	// A collection of particles and walls to plot
	std::vector<Ball*> balls;
	std::vector<Wall*> walls;

public:
	// Constructors & destructors
	Sim();
	~Sim();

	// Initialisation functions
	void initWindow();
	void initBalls();
	void initWalls();

	// General functions
	void run();
	void updateFrameTimer();
	void handleEvents();
	void updateLogic();
	void render();

	// Particle processing functions
	void collideBalls();
};

