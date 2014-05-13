#pragma once

#include "GameObject.h"

// This class basically just adds a timeout counter to GameObject. All the particle "smarts" are in Laser.
class Particle : public GameObject
{
public:
	float timeActive;

	Particle() : timeActive(0) { }
};