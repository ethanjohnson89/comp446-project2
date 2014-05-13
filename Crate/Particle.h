#pragma once

#include "GameObject.h"

// This class basically just adds lifetime support to GameObject. All the particle "smarts" are in Laser.
class Particle : public GameObject
{
public:
	float timeActive;
	float lifetime;

	Particle() : timeActive(0) { }
};