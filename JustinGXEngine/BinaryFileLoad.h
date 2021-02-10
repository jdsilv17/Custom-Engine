#pragma once

#include <vector>
#include <fstream>
#include <cassert>
#include <DirectXMath.h>

#ifndef MeshUtils_h_
#define MeshUtils_h_
#include "MeshUtils.h"
#endif // MeshUtils_h_

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

	inline void load_FBXMesh_blob(const char* path, std::vector<int>& indices_list, std::vector<VERTEX>& vertex_list)
	{
		uint32_t index_count = 0;
		uint32_t vertex_count = 0;

		std::fstream file{ path, std::ios_base::in | std::ios_base::binary };

		assert(file.is_open());

		if (!file.is_open())
			assert(false);

		if (file.is_open())
		{
			// read indices
			file.read((char*)&index_count, sizeof(uint32_t));
			indices_list.resize(index_count);
			file.read((char*)indices_list.data(), sizeof(uint32_t) * index_count);
			// read positions
			file.read((char*)&vertex_count, sizeof(uint32_t));
			vertex_list.resize(vertex_count);
			file.read((char*)vertex_list.data(), sizeof(VERTEX) * vertex_count);
		}

		file.close();

		for (auto& vert : vertex_list)
		{
			vert.pos.x = -vert.pos.x;
			//vert.pos.z = -vert.pos.z;
			vert.normals.x = -vert.normals.x;
			//vert.normals.z = -vert.normals.z;
			//vert.uv.x = 1.0f - vert.uv.x;
			vert.uv.y = 1.0f - vert.uv.y;
		}

		int tri_count = (int)(indices_list.size() / 3);
		for (size_t i = 0; i < tri_count; ++i)
		{
			int* tri = indices_list.data() + i * 3;
			int temp = tri[0];
			tri[0] = tri[2];
			tri[2] = temp;
		}
	}

	template<typename Mat, typename Path>
	inline void Load_FBXMat_blob(char const* path, std::vector<Mat>& in_mats, std::vector<Path>& in_paths)
	{
		size_t mat_count = 0;
		size_t path_count = 0;

		std::fstream file(path, std::ios_base::in | std::ios_base::binary);

		assert(file.is_open());

		if (!file.is_open())
			assert(false);

		if (file.is_open())
		{
			file.read((char*)&mat_count, sizeof(size_t));
			in_mats.resize(mat_count);
			file.read((char*)in_mats.data(), sizeof(Mat) * mat_count);

			file.read((char*)&path_count, sizeof(size_t));
			in_paths.resize(path_count);
			file.read((char*)in_paths.data(), sizeof(Path) * path_count);
		}

		file.close();
	}
}