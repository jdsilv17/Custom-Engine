#include "JustinGXEngine.h"

bool Engine::Initialize(HINSTANCE hInstance, int nCmdShow)
{

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, this->szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_JUSTINGXENGINE, this->szWindowClass, MAX_LOADSTRING);
    this->MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!this->InitInstance(hInstance, nCmdShow))
        return false;

    if (!this->GFX.Initialize(this->hWnd))
        return false;

    this->Timer.Start();

	return true;
}

void Engine::Update()
{
    Timer.GetElapsedMilliseconds(); // causes view matrix to swap rows
    Timer.Restart();
    float dt = ((float)Timer.deltaTime / 1000.0f);

    CatchInput();

    if (false /*!DrawGrid*/)
        end::MakeColorGrid(20.0f, 24, dt * 0.5f); // creates grid that changes color overtime

    //SortedPoolParticle(dt);
    //FreeListParticle(dt);

    //// Create the Target, LookAt, and TurnTo this->GFX.Gizmos
    //this->GFX.Gizmos[1].SetLookAt(this->GFX.Gizmos[1].GetPositionVector(), this->GFX.Gizmos[0].GetPositionVector(), this->GFX.Gizmos[1].UP);
    //this->GFX.Gizmos[2].SetTurnTo(this->GFX.Gizmos[2].GetWorldMatrix(), this->GFX.Gizmos[0].GetPositionVector(), dt * 0.5f);
    //// Draw this->GFX.Gizmos
    //size_t gizmo_count = this->GFX.Gizmos.size();
    //for (size_t i = 0; i < gizmo_count; ++i)
    //{
    //    XMVECTOR x = this->GFX.Gizmos[i].GetWorldMatrix().r[0] + this->GFX.Gizmos[i].GetPositionVector();
    //    XMVECTOR y = this->GFX.Gizmos[i].GetWorldMatrix().r[1] + this->GFX.Gizmos[i].GetPositionVector();
    //    XMVECTOR z = this->GFX.Gizmos[i].GetWorldMatrix().r[2] + this->GFX.Gizmos[i].GetPositionVector();
    //    XMFLOAT4 xAxis;
    //    XMStoreFloat4(&xAxis, x);
    //    XMFLOAT4 yAxis;
    //    XMStoreFloat4(&yAxis, y);
    //    XMFLOAT4 zAxis;
    //    XMStoreFloat4(&zAxis, z);

    //    // x-axis
    //    end::debug_renderer::add_line(this->GFX.Gizmos[i].GetPositionFloat4(), xAxis, { 1.0f, 0.0f, 0.0f, 1.0f });
    //    // y-axis
    //    end::debug_renderer::add_line(this->GFX.Gizmos[i].GetPositionFloat4(), yAxis, { 0.0f, 1.0f, 0.0f, 1.0f });
    //    // z-axis
    //    end::debug_renderer::add_line(this->GFX.Gizmos[i].GetPositionFloat4(), zAxis, { 0.0f, 0.0f, 1.0f, 1.0f });
    //}

    // Draw Joints in a Keyframe
    const Animation::Keyframe* frame = nullptr;
    if (this->GFX.run_anim.IsPlaying())
        frame = this->GFX.run_anim.Playback();
    else
        frame = this->GFX.run_anim.GetCurrentKeyframe();
    size_t joint_count = frame->joints.size();
    for (size_t i = 0; i < joint_count; ++i)
    {
        XMVECTOR x = frame->joints[i].jointObject.GetWorldMatrix().r[0] * 0.2f
            + frame->joints[i].jointObject.GetPositionVector();
        XMVECTOR y = frame->joints[i].jointObject.GetWorldMatrix().r[1] * 0.2f
            + frame->joints[i].jointObject.GetPositionVector();
        XMVECTOR z = frame->joints[i].jointObject.GetWorldMatrix().r[2] * 0.2f
            + frame->joints[i].jointObject.GetPositionVector();
        XMFLOAT4 xAxis;
        XMStoreFloat4(&xAxis, x);
        XMFLOAT4 yAxis;
        XMStoreFloat4(&yAxis, y);
        XMFLOAT4 zAxis;
        XMStoreFloat4(&zAxis, z);

        // x-axis
        end::debug_renderer::add_line(frame->joints[i].jointObject.GetPositionFloat4(), xAxis, { 1.0f, 0.0f, 0.0f, 1.0f });
        // y-axis
        end::debug_renderer::add_line(frame->joints[i].jointObject.GetPositionFloat4(), yAxis, { 0.0f, 1.0f, 0.0f, 1.0f });
        // z-axis
        end::debug_renderer::add_line(frame->joints[i].jointObject.GetPositionFloat4(), zAxis, { 0.0f, 0.0f, 1.0f, 1.0f });
    }

    // draw Bones
    for (size_t j = 0; j < joint_count; j++)
    {
        const Animation::Joint* child = &frame->joints[j];
        if (child->parent_index != -1)
        {
            const Animation::Joint* parent = &frame->joints[child->parent_index];
            end::debug_renderer::add_line(child->jointObject.GetPositionFloat4(), parent->jointObject.GetPositionFloat4(),
                XMFLOAT4(Colors::HotPink), XMFLOAT4(Colors::White));
        }
    }
    if (this->GFX.run_anim.IsPlaying())
        delete frame;

    #pragma region FRUSTUM CULLING
    //// Create View Frustum
    //end::frustum_t frustum;
    //end::calculate_frustum(frustum, this->GFX.Gizmos[0].GetWorldMatrix(), aspectRatio);
    //    
    //end::debug_renderer::add_line(frustum.corners[0], frustum.corners[1], XMFLOAT4(Colors::Fuchsia)); // FTL, FTR
    //end::debug_renderer::add_line(frustum.corners[1], frustum.corners[3], XMFLOAT4(Colors::Fuchsia)); // FTR, FBR
    //end::debug_renderer::add_line(frustum.corners[3], frustum.corners[2], XMFLOAT4(Colors::Fuchsia)); // FBR, FBL
    //end::debug_renderer::add_line(frustum.corners[2], frustum.corners[0], XMFLOAT4(Colors::Fuchsia)); // FBL, FTL
    //end::debug_renderer::add_line(frustum.corners[4], frustum.corners[5], XMFLOAT4(Colors::Fuchsia)); // NTL, NTR
    //end::debug_renderer::add_line(frustum.corners[5], frustum.corners[7], XMFLOAT4(Colors::Fuchsia)); // NTR, NBR
    //end::debug_renderer::add_line(frustum.corners[7], frustum.corners[6], XMFLOAT4(Colors::Fuchsia)); // NBR, NBL
    //end::debug_renderer::add_line(frustum.corners[6], frustum.corners[4], XMFLOAT4(Colors::Fuchsia)); // NBL, NTL
    //end::debug_renderer::add_line(frustum.corners[4], frustum.corners[0], XMFLOAT4(Colors::Fuchsia)); // NTL, FTL
    //end::debug_renderer::add_line(frustum.corners[6], frustum.corners[2], XMFLOAT4(Colors::Fuchsia)); // NBL, FBL
    //end::debug_renderer::add_line(frustum.corners[5], frustum.corners[1], XMFLOAT4(Colors::Fuchsia)); // NTR, FTR
    //end::debug_renderer::add_line(frustum.corners[7], frustum.corners[3], XMFLOAT4(Colors::Fuchsia)); // NBR, FBR

    //XMVECTOR planeAvg_V[6] = {};
    ////LEFT PLANE 0246
    //planeAvg_V[0] = (XMLoadFloat4(&frustum.corners[0]) + XMLoadFloat4(&frustum.corners[2]) + XMLoadFloat4(&frustum.corners[4]) + XMLoadFloat4(&frustum.corners[6])) / 4.0f;
    ////RIGHT PLANE 1357
    //planeAvg_V[1] = (XMLoadFloat4(&frustum.corners[1]) + XMLoadFloat4(&frustum.corners[3]) + XMLoadFloat4(&frustum.corners[5]) + XMLoadFloat4(&frustum.corners[7])) / 4.0f;
    ////NEAR PLANE 4567
    //planeAvg_V[2] = (XMLoadFloat4(&frustum.corners[4]) + XMLoadFloat4(&frustum.corners[5]) + XMLoadFloat4(&frustum.corners[5]) + XMLoadFloat4(&frustum.corners[6])) / 4.0f;
    ////FAR PLANE 0123
    //planeAvg_V[3] = (XMLoadFloat4(&frustum.corners[0]) + XMLoadFloat4(&frustum.corners[1]) + XMLoadFloat4(&frustum.corners[2]) + XMLoadFloat4(&frustum.corners[3])) / 4.0f;
    ////TOP PLANE 0145
    //planeAvg_V[4] = (XMLoadFloat4(&frustum.corners[0]) + XMLoadFloat4(&frustum.corners[1]) + XMLoadFloat4(&frustum.corners[4]) + XMLoadFloat4(&frustum.corners[5])) / 4.0f;
    ////BOTTOM PLANE 2367
    //planeAvg_V[5] = (XMLoadFloat4(&frustum.corners[2]) + XMLoadFloat4(&frustum.corners[3]) + XMLoadFloat4(&frustum.corners[6]) + XMLoadFloat4(&frustum.corners[7])) / 4.0f;
    //    
    //XMFLOAT4 planeAvg_F[6] = {};
    //for (size_t i = 0; i < 6; ++i)
    //{
    //    XMStoreFloat4(&planeAvg_F[i], planeAvg_V[i]);

    //    XMFLOAT4 normal = { 0.0f, 0.0f, 0.0f, 1.0f };
    //    normal.x = planeAvg_F[i].x + frustum.planes[i].normal.x;
    //    normal.y = planeAvg_F[i].y + frustum.planes[i].normal.y;
    //    normal.z = planeAvg_F[i].z + frustum.planes[i].normal.z;
    //    // Draw plane normals
    //    end::debug_renderer::add_line(planeAvg_F[i], normal, { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
    //}

    //// Create AABBs
    //for (size_t i = 0; i < ARRAYSIZE(aabbs); ++i)
    //{
    //    XMFLOAT4 color = XMFLOAT4(Colors::Cyan);
    //    if (end::aabb_to_frustum(aabbs[i], frustum))
    //        color = XMFLOAT4(Colors::Orange);

    //    Create_AABB(aabbs[i], color);
    //}
#pragma endregion

    //end::aabb_t player_aabb;
    //XMVECTOR extents = { 1.0f, 2.0f, 1.0f }/*Gizmo[0].GetWorldMatrix().r[0] + Gizmo[0].GetWorldMatrix().r[1] + Gizmo[0].GetWorldMatrix().r[2]*/;
    //XMStoreFloat3(&player_aabb.center, this->GFX.Gizmos[0].GetPositionVector());
    //XMStoreFloat3(&player_aabb.extents, extents);
    //Create_AABB(player_aabb, XMFLOAT4(Colors::Blue));

    //Create_AABB(AABB_Bounds_from_Triangle(terrain_tri_indices[0]), XMFLOAT4(Colors::Red));
    //size_t size = terrain_triangles.size();
    //for (size_t i = 0; i < size; ++i)
    //{
    //    end::debug_renderer::add_line(terrain_triangles[i][0].pos, terrain_triangles[i][1].pos, XMFLOAT4(Colors::White));
    //    end::debug_renderer::add_line(terrain_triangles[i][1].pos, terrain_triangles[i][2].pos, XMFLOAT4(Colors::White));
    //    end::debug_renderer::add_line(terrain_triangles[i][2].pos, terrain_triangles[i][0].pos, XMFLOAT4(Colors::White));
    //}

    #pragma region BVH
// Build BVH
//std::mt19937_64 g(rand());
//std::shuffle(terrain_tri_indices.begin(), terrain_tri_indices.end(), g);
//end::bvh_node_t root(AABB_Bounds_from_Triangle(terrain_tri_indices[0]), NULL);
////end::bvh_node_t root(nullptr);
//BVH.push_back(root);
//size_t size = terrain_triangles.size();
//for (size_t i = 1; i < size; ++i)
//{
//    //BVH[i];n
//    size_t tri_index = terrain_tri_indices[i];
//    end::bvh_node_t leaf_N(AABB_Bounds_from_Triangle(tri_index), tri_index);
//    //end::bvh_node_t curr(&root, tri_index - 1, tri_index + 1);
//    end::bvh_node_t curr(BVH[i - 1]);
//    while (!curr.is_leaf()) // is a root/branch
//    {
//        // Expand current bounds to include N
//        //XMVECTOR a_max = XMLoadFloat3(&curr.get_aabb().max);
//        //XMVECTOR b_max = XMLoadFloat3(&leaf_N.get_aabb().max);
//        //XMVECTOR a_min = XMLoadFloat3(&curr.get_aabb().min);
//        //XMVECTOR b_min = XMLoadFloat3(&leaf_N.get_aabb().min);
//        //XMVECTOR Max = XMVectorMax(a_max, b_max);
//        //XMVECTOR Min = XMVectorMin(a_min, b_min);
//        //end::aabb_bounds_t new_aabb = AABB_Bounds(Max, Min);
//        ////Create_AABB(new_aabb); // temp
//        //end::bvh_node_t new_node(new_aabb, i);
//        
//        // Determine which child of CurrentNode has best cost // current = cost(N, left) < cost(N, right) ? left : right
//        end::bvh_node_t left(AABB_Bounds_from_Triangle(curr.get_left()), i);
//        end::bvh_node_t right(AABB_Bounds_from_Triangle(curr.get_right()), i);
//        float left_cost = ManhattanDistance(terrain_centroids[tri_index], terrain_centroids[curr.get_left()]);
//        float right_cost = ManhattanDistance(terrain_centroids[tri_index], terrain_centroids[curr.get_right()]);
//        curr = (left_cost > right_cost) ? left : right;
//    }

//    // Create a new internal node as parent of CurrentNode and inserting leaf
//    XMVECTOR a_max = XMLoadFloat3(&curr.get_aabb().max);
//    XMVECTOR b_max = XMLoadFloat3(&leaf_N.get_aabb().max);
//    XMVECTOR a_min = XMLoadFloat3(&curr.get_aabb().min);
//    XMVECTOR b_min = XMLoadFloat3(&leaf_N.get_aabb().min);
//    XMVECTOR Max = XMVectorMax(a_max, b_max);
//    XMVECTOR Min = XMVectorMin(a_min, b_min);
//    end::aabb_bounds_t new_aabb = AABB_Bounds(Max, Min);
//    end::bvh_node_t new_node(new_aabb, i);
//    auto& p = new_node.get_parent();
//    p = i;
//    end::bvh_node_t parent_node(&new_node, curr.get_element_id(), leaf_N.get_element_id());
//    BVH.push_back(parent_node);
//    //root(parent_node);
//}
//BVH.shrink_to_fit();
#pragma endregion

}

void Engine::RenderFrame()
{
    this->GFX.RenderFrame();
}

void Engine::CleanUp()
{
    this->Timer.Stop();
    this->GFX.CleanUp();
}

/// <summary>
/// Catches the keyboard and mouse input
/// </summary>
void Engine::CatchInput()
{
    static Time uTimer;
    uTimer.GetElapsedMilliseconds();
    uTimer.Restart();

    const float cameraSpeed = 0.002f;

#pragma region OLD CAMERA ROTATION
    //POINT curr_point = { 0,0 };
    //POINT delta_point = { 0,0 };
    //
    //GetCursorPos(&curr_point); // grab the curr every frame
    //
    //static POINT prev_point = curr_point; // initialize once
    //
    //// calc delta of mouse pos with the pos of the previous frame
    //delta_point.x = curr_point.x - prev_point.x;
    //delta_point.y = curr_point.y - prev_point.y;
    //
    //prev_point = curr_point; // keep the current pos of the current frame to use in the next frame
    //
    //if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) // Right mouse button
    //{
    //    cam.UpdateRotation(static_cast<float>(delta_point.y) * 0.005f, static_cast<float>(delta_point.x) * 0.005f, 0.0f);
    //}
#pragma endregion

    if (this->GFX.bits[0]) // up arrow
    {
        this->GFX.Gizmos[0].UpdatePosition(this->GFX.Gizmos[0].GetWorldMatrix().r[2] * 0.002f * (float)uTimer.deltaTime);
        this->GFX.pntLight.UpdatePosition(0.0f, 0.002f * (float)uTimer.deltaTime, 0.0f);
    }
    if (this->GFX.bits[1]) // down arrow
    {
        this->GFX.Gizmos[0].UpdatePosition(-this->GFX.Gizmos[0].GetWorldMatrix().r[2] * 0.002f * (float)uTimer.deltaTime);
        this->GFX.pntLight.UpdatePosition(0.0f, -0.002f * (float)uTimer.deltaTime, 0.0f);
    }
    if (this->GFX.bits[2]) // left arrow
    {
        this->GFX.Gizmos[0].UpdateRotation(0.0f, -XM_PI * 0.02f, 0.0f * (float)uTimer.deltaTime);
        this->GFX.pntLight.UpdatePosition(-0.002f * (float)uTimer.deltaTime, 0.0f, 0.0f);
    }
    if (this->GFX.bits[3]) // right arrow
    {
        this->GFX.Gizmos[0].UpdateRotation(0.0f, XM_PI * 0.02f, 0.0f * (float)uTimer.deltaTime);
        this->GFX.pntLight.UpdatePosition(0.002f * (float)uTimer.deltaTime, 0.0f, 0.0f);
    }
    if (this->GFX.bits[4]) // W
    {
        this->GFX.cam.UpdatePosition(this->GFX.cam.GetForwardVector() * cameraSpeed * (float)uTimer.deltaTime);
    }
    if (this->GFX.bits[5]) // A
    {
        this->GFX.cam.UpdatePosition(this->GFX.cam.GetLeftVector() * cameraSpeed * (float)uTimer.deltaTime);
    }
    if (this->GFX.bits[6]) // S
    {
        this->GFX.cam.UpdatePosition(this->GFX.cam.GetBackwardVector() * cameraSpeed * (float)uTimer.deltaTime);
    }
    if (this->GFX.bits[7]) // D
    {
        this->GFX.cam.UpdatePosition(this->GFX.cam.GetRightVector() * cameraSpeed * (float)uTimer.deltaTime);
    }
    if (this->GFX.bits[8]) // SPACE
    {
        this->GFX.cam.UpdatePosition(this->GFX.cam.UP * cameraSpeed * (float)uTimer.deltaTime);
    }
    if (this->GFX.bits[9]) // X
    {
        this->GFX.cam.UpdatePosition(this->GFX.cam.UP * -cameraSpeed * (float)uTimer.deltaTime);
    }
    if (this->GFX.bits[10] || GetAsyncKeyState('Q') & 0x0001)
    {
        this->GFX.DrawQuad = !this->GFX.DrawQuad;
    }
    if (this->GFX.bits[11] || GetAsyncKeyState('G') & 0x0001)
    {
        this->GFX.DrawGrid = !this->GFX.DrawGrid;
    }
    //if (this->GFX.bits[12]) // < (,)
    //{
    //    run_anim.FrameStepBack();
    //}
    //if (this->GFX.bits[13]) // > (.)
    //{
    //    run_anim.FrameStepForward();
    //}
    if (this->GFX.bits[14]) // R
    {
        if (this->GFX.run_anim.IsPlaying())
            this->GFX.run_anim.StopPlayback();

        this->GFX.run_anim.ResetCurrentFrame();
    }
}

void Engine::SortedPoolParticle(float dt)
{
    // Sorted Pool Algo
    // every 0.02 secs activate a particle
    float t = (std::ceilf(dt / 0.01f));
    if (t == 2.0f)
    {
        this->GFX.sortEmitter.indices.alloc();
        this->GFX.sortPool.alloc();
    }
    for (uint16_t i = 0; i < this->GFX.sortEmitter.indices.size(); ++i) // for every active particle, update it
    {
        this->GFX.sortPool[i].prev_pos = this->GFX.sortPool[i].Pos;
        this->GFX.sortPool[i].Velocity += this->GFX.sortPool[i].Gravity * dt; // apply gravity

        XMVECTOR pos = XMLoadFloat4(&this->GFX.sortPool[i].Pos);
        pos += this->GFX.sortPool[i].Velocity * dt;
        XMStoreFloat4(&this->GFX.sortPool[i].Pos, pos); // move particle
        this->GFX.sortPool[i].Lifetime -= dt; // kill it, but slowly
        if (this->GFX.sortPool[i].Lifetime > 0.0f) // if particle is dead
            end::debug_renderer::add_line(this->GFX.sortPool[i].prev_pos, this->GFX.sortPool[i].Pos, { 1.0f, 0.0f, 0.0f, 1.0f }, this->GFX.sortEmitter.Spawn_Color);
        else
        {
            this->GFX.sortPool.free(i); // free it
            this->GFX.sortEmitter.indices.free(i);

            int16_t index = (uint16_t)this->GFX.sortPool.size();

            this->GFX.sortPool[index].Pos = this->GFX.sortEmitter.GetSpawnPositionFloat4();
            this->GFX.sortPool[index].Velocity = { RAND_FLT(-3.0f, 3.0f), 15.0f, RAND_FLT(-3.0f, 3.0f), 0.0f };
            this->GFX.sortPool[index].Lifetime = 3.0f;
        }
    }
}

void Engine::FreeListParticle(float dt)
{
    // Free List Algo
    float t = (std::ceilf(dt / 0.01f));
    for (size_t emt = 0; emt < ARRAYSIZE(this->GFX.emitters); ++emt)
    {
        int16_t emtIndex = 0;
        int16_t poolIndex = 0;
        // allocate space for a particle
        // every 0.02 secs 
        if (t == 2.0f)
        {
            emtIndex = this->GFX.emitters[emt].indices.alloc();
            poolIndex = this->GFX.sharedPool.alloc(); // alloc particle so is it ready for use
            if (poolIndex == -1)
            {
                this->GFX.sharedPool.free(poolIndex);
                break;
            }

            // Initialize the particle
            this->GFX.emitters[emt].indices[emtIndex - 1] = poolIndex; // store the indices of the ready particles
            const XMVECTOR gravity = { 0.0f, -9.8f, 0.0f };
            this->GFX.sharedPool[poolIndex].Pos = this->GFX.emitters[emt].GetSpawnPositionFloat4();
            this->GFX.sharedPool[poolIndex].Velocity = { RAND_FLT(-3.0f, 3.0f), 15.0f, RAND_FLT(-3.0f, 3.0f), 0.0f };
            this->GFX.sharedPool[poolIndex].Gravity = gravity;
            this->GFX.sharedPool[poolIndex].Lifetime = 3.0f;
        }

        for (uint16_t i = 0; i < this->GFX.emitters[emt].indices.size(); ++i) // for every active particle, update it
        {
            int16_t index = this->GFX.emitters[emt].indices[i];
            this->GFX.sharedPool[index].prev_pos = this->GFX.sharedPool[index].Pos;
            this->GFX.sharedPool[index].Velocity += this->GFX.sharedPool[index].Gravity * dt; // apply gravity // am i nothing to you

            XMVECTOR pos = XMLoadFloat4(&this->GFX.sharedPool[index].Pos);
            pos += this->GFX.sharedPool[index].Velocity * dt;
            XMStoreFloat4(&this->GFX.sharedPool[index].Pos, pos); // move particle

            this->GFX.sharedPool[index].Lifetime -= dt; // kill it, but slowly
            end::debug_renderer::add_line(this->GFX.sharedPool[index].prev_pos, this->GFX.sharedPool[index].Pos, this->GFX.emitters[emt].Spawn_Color);

            if (this->GFX.sharedPool[index].Lifetime <= 0.0f) // if particle is dead
            {
                // deallocate
                this->GFX.emitters[emt].indices.free(i);
                this->GFX.sharedPool.free(index);
            }
        }
    }
}