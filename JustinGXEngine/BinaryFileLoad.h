#pragma once

#include <vector>
#include <fstream>
#include <cassert>
#include <DirectXMath.h>

namespace load_binary
{
	inline std::vector<uint8_t> load_binary_blob(const char* path)
	{
		std::vector<uint8_t> blob;

		std::fstream file{ path, std::ios_base::in | std::ios_base::binary };

		if (file.is_open())
		{
			file.seekg(0, std::ios_base::end);
			blob.resize(file.tellg());
			file.seekg(0, std::ios_base::beg);

			file.read((char*)blob.data(), blob.size());

			file.close();
		}

		return std::move(blob);
	}

	inline std::vector<DirectX::XMFLOAT3> load_terrain_blob(const char* path)
	{
		std::fstream file{ path, std::ios_base::in | std::ios_base::binary };
		std::vector<DirectX::XMFLOAT3> pos;

		assert(file.is_open());

		if (!file.is_open())
			assert(false);

		if (file.is_open())
		{
			uint32_t terrain_vert_count = NULL;
			file.read((char*)&terrain_vert_count, sizeof(uint32_t));

			pos.resize(terrain_vert_count);
			//norms.resize(terrain_vert_count);
			//uvs.resize(terrain_vert_count);

			file.read((char*)pos.data(), sizeof(DirectX::XMFLOAT3) * terrain_vert_count);
			//file.read((char*)norms.data(), sizeof(DirectX::XMFLOAT3) * terrain_vert_count);
			//file.read((char*)uvs.data(), sizeof(DirectX::XMFLOAT2) * terrain_vert_count);
		}

		file.close();


		return std::move(pos);
	}
}