# sfml-dynamics

A rigid-body dynamics engine in C++, with graphics implemented using SFML. Currently includes bouncing balls, and walls that can move & rotate.

## Implementation details

To determine the outcome of a collision, the program first uses the geometry of the colliding bodies to find the collision normal and tangent vectors, then resolves the velocities of the colliding objects along these directions.

The normal components of the velocities are set according to conservation of momentum, with energy losses parametrised by a coefficient of restitution e. Next, the tangential components are set according to F ≤ μR, where μ is a coefficient of friction, R is the normal impulse that was applied, and F is the frictional impulse that must be applied.

Implementation of the collision handling can be found in [Ball.cpp](sfml-dynamics/Ball.cpp).

![Example screenshot](sfml-dynamics-screenshot.png)


