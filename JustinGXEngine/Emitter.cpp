#include "Emitter.h"

const DirectX::XMVECTOR& Emitter::GetSpawnPositionVector() const
{
	return this->GetPositionVector();
}

const DirectX::XMFLOAT4& Emitter::GetSpawnPositionFloat4() const
{
	return this->GetPositionFloat4();
}

void Emitter::SetSpawnPosition(const DirectX::XMVECTOR& pos)
{
	this->SetPosition(pos);
}

void Emitter::SetSpawnPosition(float x, float y, float z)
{
	this->SetPosition(x, y, z);
}
