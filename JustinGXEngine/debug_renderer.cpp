#include "debug_renderer.h"


#include <array>

// Anonymous namespace
namespace
{
	// Declarations in an anonymous namespace are global BUT only have internal linkage.
	// In other words, these variables are global but are only visible in this source file.

	// Maximum number of debug lines at one time (i.e: Capacity)
	constexpr size_t MAX_LINE_VERTS = 4096; 

	// CPU-side buffer of debug-line verts
	// Copied to the GPU and reset every frame.
	size_t line_vert_count = 0;
    std::array<VERTEX, MAX_LINE_VERTS> line_verts;
}

namespace end
{
	namespace debug_renderer
	{
		void add_line(DirectX::XMFLOAT4 point_a, DirectX::XMFLOAT4 point_b, DirectX::XMFLOAT4 color_a, DirectX::XMFLOAT4 color_b)
		{
            //if (line_verts.size() < MAX_LINE_VERTS)
            //    line_verts.resize(MAX_LINE_VERTS);

            if (line_vert_count == MAX_LINE_VERTS)
                return;
			// Add points to debug_verts, increments debug_vert_count
			line_verts[line_vert_count] = VERTEX(point_a, color_a);
			++line_vert_count;
			line_verts[line_vert_count] = VERTEX(point_b, color_b);
			++line_vert_count;
		}

		void clear_lines()
		{
			// Resets debug_vert_count
			line_vert_count = 0;
		}

		const VERTEX* get_line_verts()
		{ 
			// Does just what it says in the name
			return line_verts.data();
		}

		size_t get_line_vert_count() 
		{ 
			// Does just what it says in the name
			return line_vert_count;
		}

		size_t get_line_vert_capacity()
		{
			// Does just what it says in the name
			return MAX_LINE_VERTS;
		}
	}

    void MakeColorGrid(float gridSize, int lineCount, float deltaTime)
    {
        //std::array<VERTEX, MAX_LINE_VERTS> lines;

        // need: size, spacing, linecount, 
        int maxVerts = 2048;
        if (lineCount * 4 > maxVerts)
            lineCount = 510;

        float lineSpacing = gridSize / static_cast<float>(lineCount);

        float x = -gridSize / 2.0f; // starting point
        float z = -gridSize / 2.0f; // starting point

        DirectX::XMVECTOR zero = { 0.0f, 0.0f, 0.0f, 1.0f };
        DirectX::XMVECTOR one = { 1.0f, 1.0f, 1.0f, 1.0f };
        static DirectX::XMFLOAT4 color = { 1.0f, 0.0f, 0.0f, 1.0f };

        if ( (color.z <= 0.0f) && (color.x <= 1.0f && color.x > 0.0f) && (color.y >= 0.0f && color.y < 1.0f) )
        {
            color.x -= deltaTime;
            color.y += deltaTime;
        }
        else if ( (color.x <= 0.0f) && (color.y <= 1.0f && color.y > 0.0f) && (color.z >= 0.0f && color.z < 1.0f) )
        {
            color.y -= deltaTime;
            color.z += deltaTime;
        }
        else if ( (color.y <= 0.0f) && (color.z <= 1.0f && color.z > 0.0f) && (color.x >= 0.0f && color.x < 1.0f) )
        {
            color.z -= deltaTime;
            color.x += deltaTime;
        }
        DirectX::XMStoreFloat4(&color, DirectX::XMVectorClamp(DirectX::XMLoadFloat4(&color), zero, one));


        // create lines along x-axis
        for (int i = 0; i <= lineCount; ++i)
        {
            // create the line
            if (i == lineCount / 2)
            {
                debug_renderer::add_line({ x, 0.0f, z + gridSize, 1.0f }, { x, 0.0f, z, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
            }
            else
            {
                debug_renderer::add_line({ x, 0.0f, z + gridSize, 1.0f }, { x, 0.0f, z, 1.0f }, color);
            }
            // move over on the x-axis by the spacing
            x += lineSpacing;
        }
        x = -gridSize / 2.0f;
        // create lines along z-axis
        for (int i = 0; i <= lineCount; ++i)
        {
            // create the line
            if (i == lineCount / 2)
            {
                debug_renderer::add_line({ x, 0.0f, z, 1.0f }, { x + gridSize, 0.0f, z, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
            }
            else
            {
                debug_renderer::add_line({ x, 0.0f, z, 1.0f }, { x + gridSize, 0.0f, z, 1.0f }, color);
            }
            // move over on the x-axis by the spacing
            z += lineSpacing;
        }
    }
}