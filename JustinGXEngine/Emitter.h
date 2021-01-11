#pragma once

#include "Particle.h"
#include "pools.h"

class Emitter :
	private Object
{
public:
	Emitter() {}
	~Emitter() {}

	const DirectX::XMVECTOR& GetSpawnPositionVector() const;
	const DirectX::XMFLOAT4& GetSpawnPositionFloat4() const;

	void SetSpawnPosition(const DirectX::XMVECTOR& pos);
	void SetSpawnPosition(float x, float y, float z);

	DirectX::XMFLOAT4 Spawn_Color = { 1.0f, 1.0f, 1.0f, 1.0f };

	// indices into the shared pool
	end::Sorted_Pool_t<int16_t, 256> indices;

private:

};

