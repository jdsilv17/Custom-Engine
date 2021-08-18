#pragma once

#include <DirectXMath.h>
#include <array>
using namespace DirectX;
// Note: You are free to make adjustments/additions to the declarations provided here.

namespace end
{
	//using sphere_t = DirectX::XMFLOAT4;
	struct sphere_t { DirectX::XMFLOAT3 center; float radius; }; //Alterative: using sphere_t = float4;

	struct aabb_t { DirectX::XMFLOAT3 center; DirectX::XMFLOAT3 extents; }; //Alternative: aabb_t { float3 min; float3 max; };
	
	//using plane_t = DirectX::XMFLOAT4;
	struct plane_t { DirectX::XMFLOAT3 normal; float offset; };  //Alterative: using plane_t = float4;

	//using frustum_t = std::array<plane_t, 6>;
    struct frustum_t
    {
        std::array<plane_t, 6> planes;
        std::array<DirectX::XMFLOAT4, 8> corners;
    };

	// Calculates the plane of a triangle from three points.
	inline plane_t calculate_plane(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 c)
	{
		plane_t plane;
		const DirectX::XMVECTOR origin = { 0.0f, 0.0f, 0.0f };
		DirectX::XMVECTOR _a = DirectX::XMLoadFloat3(&a);
		DirectX::XMVECTOR _b = DirectX::XMLoadFloat3(&b);
		DirectX::XMVECTOR _c = DirectX::XMLoadFloat3(&c);

		// get the vectors for the edge of the triangle
		DirectX::XMVECTOR U = _b - _a;
		DirectX::XMVECTOR V = _c - _b;

		// cross product to find the surface normal
		DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(U, V));
		DirectX::XMStoreFloat3(&plane.normal, normal);

		// dot plane normal with a point to find the offset
		plane.offset = DirectX::XMVector3Dot(normal, _a).m128_f32[0];

		return plane;
	}

	// Calculates a frustum (6 planes) from the input view parameter.
	//
	// Calculate the eight corner points of the frustum. 
	// Use your debug renderer to draw the edges.
	// 
	// Calculate the frustum planes.
	// Use your debug renderer to draw the plane normals as line segments.
    inline void calculate_frustum(frustum_t& frustum, const DirectX::XMMATRIX& view, float& aspectRatio)
	{
        XMFLOAT4 NTL;   //Near - Top - Left(NTL)
        XMFLOAT4 NTR;   //Near - Top - Right(NTR)
        XMFLOAT4 NBL;   //Near - Bottom - Left(NBL)
        XMFLOAT4 NBR;   //Near - Bottom - Right(NBR)

        XMFLOAT4 FTL;   //Far - Top - Left(FTL)
        XMFLOAT4 FTR;   //Far - Top - Right(FTR)
        XMFLOAT4 FBL;   //Far - Bottom - Left(FBL)
        XMFLOAT4 FBR;   //Far - Bottom - Right(FBR)
        XMFLOAT4* corners_F[8] =
        {
            &FTL,
            &FTR,
            &FBL,
            &FBR,

            &NTL,
            &NTR,
            &NBL,
            &NBR
        };
        XMVECTOR corners_V[8];

        // Formula Reference: https://stackoverflow.com/questions/13665932/calculating-the-viewing-frustum-in-a-3d-space //
        // compute the center points of the near and far planes
        float nearDistance = 1.0f;
        float farDistance = 7.0f;
        //  plane center = cameras position + cameras forward * respective plane distnace
        XMVECTOR nearCenter = view.r[3] + view.r[2] * nearDistance;
        XMVECTOR farCenter = view.r[3] + view.r[2] * farDistance;

        // Compute the widths and heights of the near and far planes:
        float fovRadians = 30.0f * (XM_PI / 180.0f);
        float nearHeight = 2.0f * tan(fovRadians / 2.0f) * nearDistance;
        float farHeight = 2.0f * tan(fovRadians / 2.0f) * farDistance;
        float nearWidth = nearHeight * aspectRatio;
        float farWidth = farHeight * aspectRatio;

        // Compute the corner points from the near and far planes:
        // corner point = plane center +/- cameras UP/Y * (plane height/2) +/- cameras RIGHT/X * (plane width/2);
        // Far top left
        corners_V[0] = farCenter + view.r[1] * (0.5f * farHeight) - view.r[0] * (0.5f * farWidth);
        // Far top right
        corners_V[1] = farCenter + view.r[1] * (0.5f * farHeight) + view.r[0] * (0.5f * farWidth);
        // Far bottom left
        corners_V[2] = farCenter - view.r[1] * (0.5f * farHeight) - view.r[0] * (0.5f * farWidth);
        // Far bottom right
        corners_V[3] = farCenter - view.r[1] * (0.5f * farHeight) + view.r[0] * (0.5f * farWidth);
        // Near top left
        corners_V[4] = nearCenter + view.r[1] * (0.5f * nearHeight) - view.r[0] * (0.5f * nearWidth);
        // Near top right
        corners_V[5] = nearCenter + view.r[1] * (0.5f * nearHeight) + view.r[0] * (0.5f * nearWidth);
        // Near bottom left
        corners_V[6] = nearCenter - view.r[1] * (0.5f * nearHeight) - view.r[0] * (0.5f * nearWidth);
        // Near bottom right
        corners_V[7] = nearCenter - view.r[1] * (0.5f * nearHeight) + view.r[0] * (0.5f * nearWidth);

        for (size_t i = 0; i < 8; ++i)
        {
            XMStoreFloat4(corners_F[i], corners_V[i]);
            XMStoreFloat4(&frustum.corners[i], corners_V[i]);
        }

        // LEFT PLANE
        frustum.planes[0] = calculate_plane(    XMFLOAT3(NBL.x, NBL.y, NBL.z), 
                                                XMFLOAT3(FTL.x, FTL.y, FTL.z), 
                                                XMFLOAT3(FBL.x, FBL.y, FBL.z)   );
        // RIGHT PLANE
        frustum.planes[1] = calculate_plane(    XMFLOAT3(FBR.x, FBR.y, FBR.z),
                                                XMFLOAT3(NTR.x, NTR.y, NTR.z),
                                                XMFLOAT3(NBR.x, NBR.y, NBR.z)   );
        // NEAR PLANE
        frustum.planes[2] = calculate_plane(    XMFLOAT3(NBR.x, NBR.y, NBR.z),
                                                XMFLOAT3(NTL.x, NTL.y, NTL.z),
                                                XMFLOAT3(NBL.x, NBL.y, NBL.z)   );
        // FAR PLANE
        frustum.planes[3] = calculate_plane(    XMFLOAT3(FBL.x, FBL.y, FBL.z),
                                                XMFLOAT3(FTR.x, FTR.y, FTR.z),
                                                XMFLOAT3(FBR.x, FBR.y, FBR.z)   );
        // TOP PLANE
        frustum.planes[4] = calculate_plane(    XMFLOAT3(FTR.x, FTR.y, FTR.z),
                                                XMFLOAT3(NTL.x, NTL.y, NTL.z),
                                                XMFLOAT3(NTR.x, NTR.y, NTR.z)   );
        // BOTTOM PLANE
        frustum.planes[5] = calculate_plane(    XMFLOAT3(NBR.x, NBR.y, NBR.z),
                                                XMFLOAT3(FBL.x, FBL.y, FBL.z),
                                                XMFLOAT3(FBR.x, FBR.y, FBR.z)   );

	}

	// Calculates which side of a plane the sphere is on.
	//
	// Returns -1 if the sphere is completely behind the plane.
	// Returns 1 if the sphere is completely in front of the plane.
	// Otherwise returns 0 (Sphere overlaps the plane)
    inline int classify_sphere_to_plane(const sphere_t& sphere, const plane_t& plane)
	{
        XMVECTOR s_Center = XMLoadFloat3(&sphere.center);
        XMVECTOR p_Normal = XMLoadFloat3(&plane.normal);
        float delta = XMVector3Dot(s_Center, p_Normal).m128_f32[0] - plane.offset;

        if (delta > sphere.radius)
            return 1;
        else if (delta < -sphere.radius)
            return -1;

        return 0;
	}

	// Calculates which side of a plane the aabb is on.
	//
	// Returns -1 if the aabb is completely behind the plane.
	// Returns 1 if the aabb is completely in front of the plane.
	// Otherwise returns 0 (aabb overlaps the plane)
	// MUST BE IMPLEMENTED UsING THE PROJECTED RADIUS TEST
    inline int classify_aabb_to_plane(const aabb_t& aabb, const plane_t& plane)
	{
        XMVECTOR a_Extents = XMLoadFloat3(&aabb.extents);
        XMVECTOR p_Normal = XMVectorAbs(XMLoadFloat3(&plane.normal));

        float projected_radius = XMVector3Dot(p_Normal, a_Extents).m128_f32[0];

        sphere_t sphere;
        sphere.center = aabb.center;
        sphere.radius = projected_radius;

        return classify_sphere_to_plane(sphere, plane);
	}

	// Determines if the aabb is inside the frustum.
	//
	// Returns false if the aabb is completely behind any plane.
	// Otherwise returns true.
    inline bool aabb_to_frustum(const aabb_t& aabb, const frustum_t& frustum)
	{
        for (size_t i = 0; i < 6; ++i)
        {
            if (classify_aabb_to_plane(aabb, frustum.planes[i]) == -1)
                return false;
        }
        
        return true;
	}
}