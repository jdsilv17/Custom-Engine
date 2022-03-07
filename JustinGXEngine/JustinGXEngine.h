#pragma once

#include "WindowContainer.h"
#include "Time.h"

class Engine : 
	WindowContainer
{
public:
    bool Initialize(HINSTANCE hInstance, int nCmdShow);
    void Update();
    void RenderFrame();
    // temp
    void CleanUp();

public:
    static Engine* Get(); // makes this class a singleton

private:
    Engine() {};
    void CatchInput();
    // temp
    void SortedPoolParticle(float dt);
    void FreeListParticle(float dt);

    Time Timer;
};

#pragma region END FUNCTIONS
//end::aabb_bounds_t AABB_Bounds_from_Triangle(int triangle_index)
//{
//    end::aabb_bounds_t aabb;
//    XMVECTOR a, b, c;
//    XMVECTOR max, min;
//
//    a = XMLoadFloat4(&terrain_triangles[triangle_index][0].pos);
//    b = XMLoadFloat4(&terrain_triangles[triangle_index][1].pos);
//    c = XMLoadFloat4(&terrain_triangles[triangle_index][2].pos);
//
//    max = XMVectorMax(XMVectorMax(a, b), XMVectorMax(a, c));
//    max.m128_f32[1] += 2.0f;
//    min = XMVectorMin(XMVectorMin(a, b), XMVectorMin(a, c));
//    min.m128_f32[1] -= 2.0f;
//    XMStoreFloat3(&aabb.max, max);
//    XMStoreFloat3(&aabb.min, min);
//
//    return aabb;
//}
//
//end::aabb_bounds_t AABB_Bounds(XMVECTOR& max, XMVECTOR& min)
//{
//    end::aabb_bounds_t aabb;
//
//    //max.m128_f32[1] += 2.0f;
//    //min.m128_f32[1] -= 2.0f;
//    XMStoreFloat3(&aabb.max, max);
//    XMStoreFloat3(&aabb.min, min);
//
//    return aabb;
//}
//
//end::aabb_t AABB_from_Triangle(int triangle_index)
//{
//    end::aabb_t aabb;
//    XMVECTOR a, b, c;
//    XMVECTOR max;
//    XMVECTOR center, extents;
//
//    a = XMLoadFloat4(&terrain_triangles[triangle_index][0].pos);
//    b = XMLoadFloat4(&terrain_triangles[triangle_index][1].pos);
//    c = XMLoadFloat4(&terrain_triangles[triangle_index][2].pos);
//
//    center = XMLoadFloat4(&terrain_centroids[triangle_index]);
//
//    max = XMVectorMax(XMVectorMax(a, b), XMVectorMax(a, c));
//    max.m128_f32[1] += 2.0f;
//    extents = max - center;
//
//    XMStoreFloat3(&aabb.center, center);
//    XMStoreFloat3(&aabb.extents, extents);
//
//    return aabb;
//}
//
//void Create_AABB(const end::aabb_t& aabb)
//{
//    float x = aabb.extents.x;
//    float y = aabb.extents.y;
//    float z = aabb.extents.z;
//    XMFLOAT4 ftr = { aabb.center.x + x, aabb.center.y + y, aabb.center.z + z, 1.0f };
//    XMFLOAT4 ftl = { aabb.center.x - x, aabb.center.y + y, aabb.center.z + z, 1.0f };
//    XMFLOAT4 fbl = { aabb.center.x - x, aabb.center.y - y, aabb.center.z + z, 1.0f };
//    XMFLOAT4 fbr = { aabb.center.x + x, aabb.center.y - y, aabb.center.z + z, 1.0f };
//    XMFLOAT4 ntl = { aabb.center.x - x, aabb.center.y + y, aabb.center.z - z, 1.0f };
//    XMFLOAT4 ntr = { aabb.center.x + x, aabb.center.y + y, aabb.center.z - z, 1.0f };
//    XMFLOAT4 nbl = { aabb.center.x - x, aabb.center.y - y, aabb.center.z - z, 1.0f };
//    XMFLOAT4 nbr = { aabb.center.x + x, aabb.center.y - y, aabb.center.z - z, 1.0f };
//
//    XMFLOAT4 color = XMFLOAT4(Colors::Red);
//    end::debug_renderer::add_line(ftl, ftr, color);
//    end::debug_renderer::add_line(ftr, fbr, color);
//    end::debug_renderer::add_line(fbr, fbl, color);
//    end::debug_renderer::add_line(fbl, ftl, color);
//
//    end::debug_renderer::add_line(ntl, ntr, color);
//    end::debug_renderer::add_line(ntr, nbr, color);
//    end::debug_renderer::add_line(nbr, nbl, color);
//    end::debug_renderer::add_line(nbl, ntl, color);
//
//    end::debug_renderer::add_line(ftl, ntl, color);
//    end::debug_renderer::add_line(ftr, ntr, color);
//    end::debug_renderer::add_line(fbl, nbl, color);
//    end::debug_renderer::add_line(fbr, nbr, color);
//}
//
//void Create_AABB(const end::aabb_t& aabb, XMFLOAT4 color)
//{
//    float x = aabb.extents.x;
//    float y = aabb.extents.y;
//    float z = aabb.extents.z;
//    XMFLOAT4 ftr = { aabb.center.x + x, aabb.center.y + y, aabb.center.z + z, 1.0f };
//    XMFLOAT4 ftl = { aabb.center.x - x, aabb.center.y + y, aabb.center.z + z, 1.0f };
//    XMFLOAT4 fbl = { aabb.center.x - x, aabb.center.y - y, aabb.center.z + z, 1.0f };
//    XMFLOAT4 fbr = { aabb.center.x + x, aabb.center.y - y, aabb.center.z + z, 1.0f };
//    XMFLOAT4 ntl = { aabb.center.x - x, aabb.center.y + y, aabb.center.z - z, 1.0f };
//    XMFLOAT4 ntr = { aabb.center.x + x, aabb.center.y + y, aabb.center.z - z, 1.0f };
//    XMFLOAT4 nbl = { aabb.center.x - x, aabb.center.y - y, aabb.center.z - z, 1.0f };
//    XMFLOAT4 nbr = { aabb.center.x + x, aabb.center.y - y, aabb.center.z - z, 1.0f };
//
//    end::debug_renderer::add_line(ftl, ftr, color);
//    end::debug_renderer::add_line(ftr, fbr, color);
//    end::debug_renderer::add_line(fbr, fbl, color);
//    end::debug_renderer::add_line(fbl, ftl, color);
//
//    end::debug_renderer::add_line(ntl, ntr, color);
//    end::debug_renderer::add_line(ntr, nbr, color);
//    end::debug_renderer::add_line(nbr, nbl, color);
//    end::debug_renderer::add_line(nbl, ntl, color);
//
//    end::debug_renderer::add_line(ftl, ntl, color);
//    end::debug_renderer::add_line(ftr, ntr, color);
//    end::debug_renderer::add_line(fbl, nbl, color);
//    end::debug_renderer::add_line(fbr, nbr, color);
//}
//
//void Create_AABB(const end::aabb_bounds_t& aabb)
//{
//    float max_x = aabb.max.x;
//    float max_y = aabb.max.y;
//    float max_z = aabb.max.z;
//    float min_x = aabb.min.x;
//    float min_y = aabb.min.y;
//    float min_z = aabb.min.z;
//    XMFLOAT4 ftl = { min_x, max_y, max_z, 1.0f };
//    XMFLOAT4 ftr = { max_x, max_y, max_z, 1.0f };//max
//    XMFLOAT4 fbl = { min_x, min_y, max_z, 1.0f };
//    XMFLOAT4 fbr = { max_x, min_y, max_z, 1.0f };
//    XMFLOAT4 ntl = { min_x, max_y, min_z, 1.0f };
//    XMFLOAT4 ntr = { max_x, max_y, min_z, 1.0f };
//    XMFLOAT4 nbl = { min_x, min_y, min_z, 1.0f };//min
//    XMFLOAT4 nbr = { max_x, min_y, min_z, 1.0f };
//
//    XMFLOAT4 color = XMFLOAT4(Colors::Red);
//    end::debug_renderer::add_line(ftl, ftr, color);
//    end::debug_renderer::add_line(ftr, fbr, color);
//    end::debug_renderer::add_line(fbr, fbl, color);
//    end::debug_renderer::add_line(fbl, ftl, color);
//
//    end::debug_renderer::add_line(ntl, ntr, color);
//    end::debug_renderer::add_line(ntr, nbr, color);
//    end::debug_renderer::add_line(nbr, nbl, color);
//    end::debug_renderer::add_line(nbl, ntl, color);
//
//    end::debug_renderer::add_line(ftl, ntl, color);
//    end::debug_renderer::add_line(ftr, ntr, color);
//    end::debug_renderer::add_line(fbl, nbl, color);
//    end::debug_renderer::add_line(fbr, nbr, color);
//}
//
//void Create_AABB(const end::aabb_bounds_t& aabb, XMFLOAT4 color)
//{
//    float max_x = aabb.max.x;
//    float max_y = aabb.max.y;
//    float max_z = aabb.max.z;
//    float min_x = aabb.min.x;
//    float min_y = aabb.min.y;
//    float min_z = aabb.min.z;
//    XMFLOAT4 ftl = { min_x, max_y, max_z, 1.0f };
//    XMFLOAT4 ftr = { max_x, max_y, max_z, 1.0f };//max
//    XMFLOAT4 fbl = { min_x, min_y, max_z, 1.0f };
//    XMFLOAT4 fbr = { max_x, min_y, max_z, 1.0f };
//    XMFLOAT4 ntl = { min_x, max_y, min_z, 1.0f };
//    XMFLOAT4 ntr = { max_x, max_y, min_z, 1.0f };
//    XMFLOAT4 nbl = { min_x, min_y, min_z, 1.0f };//min
//    XMFLOAT4 nbr = { max_x, min_y, min_z, 1.0f };
//
//    end::debug_renderer::add_line(ftl, ftr, color);
//    end::debug_renderer::add_line(ftr, fbr, color);
//    end::debug_renderer::add_line(fbr, fbl, color);
//    end::debug_renderer::add_line(fbl, ftl, color);
//
//    end::debug_renderer::add_line(ntl, ntr, color);
//    end::debug_renderer::add_line(ntr, nbr, color);
//    end::debug_renderer::add_line(nbr, nbl, color);
//    end::debug_renderer::add_line(nbl, ntl, color);
//
//    end::debug_renderer::add_line(ftl, ntl, color);
//    end::debug_renderer::add_line(ftr, ntr, color);
//    end::debug_renderer::add_line(fbl, nbl, color);
//    end::debug_renderer::add_line(fbr, nbr, color);
//}
//
//void Create_AABB(XMFLOAT3 _max, XMFLOAT3 _min)
//{
//    float max_x = _max.x;
//    float max_y = _max.y;
//    float max_z = _max.z;
//    float min_x = _min.x;
//    float min_y = _min.y;
//    float min_z = _min.z;
//    XMFLOAT4 ftl = { min_x, max_y, max_z, 1.0f };
//    XMFLOAT4 ftr = { max_x, max_y, max_z, 1.0f };//max
//    XMFLOAT4 fbl = { min_x, min_y, max_z, 1.0f };
//    XMFLOAT4 fbr = { max_x, min_y, max_z, 1.0f };
//    XMFLOAT4 ntl = { min_x, max_y, min_z, 1.0f };
//    XMFLOAT4 ntr = { max_x, max_y, min_z, 1.0f };
//    XMFLOAT4 nbl = { min_x, min_y, min_z, 1.0f };//min
//    XMFLOAT4 nbr = { max_x, min_y, min_z, 1.0f };
//
//    XMFLOAT4 color = XMFLOAT4(Colors::Red);
//    end::debug_renderer::add_line(ftl, ftr, color);
//    end::debug_renderer::add_line(ftr, fbr, color);
//    end::debug_renderer::add_line(fbr, fbl, color);
//    end::debug_renderer::add_line(fbl, ftl, color);
//
//    end::debug_renderer::add_line(ntl, ntr, color);
//    end::debug_renderer::add_line(ntr, nbr, color);
//    end::debug_renderer::add_line(nbr, nbl, color);
//    end::debug_renderer::add_line(nbl, ntl, color);
//
//    end::debug_renderer::add_line(ftl, ntl, color);
//    end::debug_renderer::add_line(ftr, ntr, color);
//    end::debug_renderer::add_line(fbl, nbl, color);
//    end::debug_renderer::add_line(fbr, nbr, color);
//}
//
//float ManhattanDistance(const XMFLOAT4& point_a, const XMFLOAT4& point_b)
//{
//    float x_dist = fabs(point_a.x - point_b.x);
//    float y_dist = fabs(point_a.y - point_b.y);
//    float z_dist = fabs(point_a.z - point_b.z);
//
//    float manhat = x_dist + y_dist + z_dist;
//
//    return manhat;
//}
#pragma endregion
