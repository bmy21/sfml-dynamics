#include "MathUtils.h"

// Utility function to wrap angles from 0->2pi
float wrap(float theta)
{
	theta = fmod(theta, 2 * pi);
	if (theta < 0) theta += 2 * pi;

	return theta;
}
