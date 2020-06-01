#pragma once

#include "SFML/System/Vector2.hpp"

#include <cmath>

// Some quick utility functions to operate on SFML vectors:
// dot product, magnitude, normalisation and angle between vectors

template<typename T>
T dotProduct(const sf::Vector2<T>& v1, const sf::Vector2<T>& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

template<typename T>
T vectorMag(const sf::Vector2<T>& v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

template<typename T>
sf::Vector2<T> normalise(const sf::Vector2<T>& v)
{
	return v / vectorMag(v);
}


template<typename T>
float angleBetween(const sf::Vector2<T>& v1, const sf::Vector2<T>& v2)
{
	return acos(dotProduct(v1, v2) / (vectorMag(v1) * vectorMag(v2)));
}