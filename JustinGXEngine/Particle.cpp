#include "Particle.h"

Particle::Particle()
{
	this->Pos = this->prev_pos = { 0.0f, 0.0f, 0.0f, 1.0f };
	this->Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	this->Velocity = { 0.0f, 15.0f, 0.0f };
	this->Gravity = { 0.0f, -9.8f, 0.0f };
	this->Lifetime = 10.0f;
}

Particle::Particle(const Particle& that)
{
	*this = that;
}

Particle& Particle::operator=(const Particle& that)
{
	if (this != &that)
	{
		this->Pos = that.Pos;
		this->Color = that.Color;
		this->prev_pos = that.prev_pos;
		this->Velocity = that.Velocity;
		this->Gravity = that.Gravity;
		this->Lifetime = that.Lifetime;
	}
	return *this;
}
