#pragma once

#include "Particle.h"
#include "pools.h"

class Emitter :
	public Object
{
public:

	DirectX::XMFLOAT4 Spawn_Color;

	end::Sorted_Pool_t<int16_t, 256> indices;

private:

};

