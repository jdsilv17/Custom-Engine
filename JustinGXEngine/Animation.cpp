#include "Animation.h"

const double Animation::Animation::GetAnimationLegnth() const
{
	return this->Duration;
}

const int Animation::Animation::GetFrameCount() const
{
	return this->FrameCount;
}

std::vector<Animation::Keyframe>& Animation::Animation::GetKeyframes()
{
	return this->Frames;
}

const Animation::Keyframe* Animation::Animation::p_GetKeyframes() const
{
	return this->Frames.data();
}

void Animation::Animation::SetAnimationLength(const double& _duration)
{
	this->Duration = _duration;
}

void Animation::Animation::SetFrameCount(const int& _frameCount)
{
	this->FrameCount = _frameCount;
}

void Animation::Animation::SetKeyframes(const std::vector<Keyframe>& _frames)
{
	this->Frames = _frames;
}

void Animation::Animation::SetKeyframes(const Keyframe* _frames, size_t numOfKeyframes)
{
	memcpy(this->Frames.data(), _frames, sizeof(Keyframe) * numOfKeyframes);
}
