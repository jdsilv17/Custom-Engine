#pragma once

#include <vector>
#include <DirectXMath.h>

#include "Object.h"

namespace Animation
{
    struct Joint
    {
        DirectX::XMFLOAT4X4 global_xform;

        int parent_index;

        Object jointObject;

        Joint() : global_xform(), parent_index(0), jointObject() {}
        Joint(DirectX::XMFLOAT4X4 _transform, int _parent)
            : global_xform(_transform), parent_index(_parent), jointObject(_transform) {}
            
    };

    struct Keyframe
    {
        double time = 0;

        std::vector<Joint> joints;
    };

    class Animation
    {
    public:
        Animation() {}

        const double GetAnimationLegnth() const;
        const int GetFrameCount() const;
        std::vector<Keyframe>& GetKeyframes();
        const Keyframe* p_GetKeyframes() const;
        const Keyframe GetKeyframe(int _index) const;

        void SetAnimationLength(const double& _duration);
        void SetFrameCount(const int& _frameCount);
        void SetKeyframes(const std::vector<Keyframe>& _frames);
        void SetKeyframes(const Keyframe* _frames, size_t numOfKeyframes);

        void Playback(double deltaTime);

        std::vector<Keyframe> Frames;

    private:
        double Duration = 0;
        int FrameCount = 0;

        
    };
}

