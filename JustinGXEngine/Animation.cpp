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

const Animation::Keyframe* Animation::Animation::GetKeyframe(int _index) const
{
	return &this->Frames[_index];
}

const Animation::Keyframe* Animation::Animation::GetCurrentKeyframe() const
{
	return &this->Frames[this->CurrentFrame];
}

const Animation::Keyframe* Animation::Animation::AnimationStart() const
{
	return this->GetKeyframe(1);
}

const Animation::Keyframe* Animation::Animation::AnimationEnd() const
{
	//return &this->Frames.back();
	return this->GetKeyframe(this->FrameCount - 1);
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

void Animation::Animation::SetCurrentFrameTo(int _index)
{
	this->CurrentFrame = _index;
}

const Animation::Keyframe* Animation::Animation::Playback()
{
	this->Timer.Start();
	this->TimeElapsed += this->Timer.GetElapsedMilliseconds() / 1000.0;
	this->Timer.Restart();

	if (this->TimeElapsed >= this->AnimationEnd()->time) // dont know if need this
	{
		this->SetCurrentFrameTo(1);
		this->TimeElapsed = this->GetCurrentKeyframe()->time;
	}

	Keyframe* tween = new Keyframe();
	const Keyframe* curr = nullptr;
	const Keyframe* next = nullptr;

	// find the 2 frames timeElasped falls b/w
	//int pos = static_cast<int>(fmin(this->TimeElapsed / this->Duration, 1.0) * (double)(this->FrameCount - 1));
	//if (pos == this->FrameCount)
	//{
	//	this->SetCurrentFrameTo(pos);
	//	curr = this->GetCurrentKeyframe();
	//	next = this->GetKeyframe(1);
	//}
	//else if (pos == 0)
	//{
	//	this->SetCurrentFrameTo(1);
	//	curr = this->GetCurrentKeyframe();
	//	next = this->GetKeyframe(2);
	//}
	//else
	//{
	//	this->SetCurrentFrameTo(pos);
	//	curr = this->GetCurrentKeyframe();
	//	next = this->GetKeyframe(pos + 1);
	//}

	for (int i = 1; i < this->FrameCount; ++i)
	{
		double currTime = 0;
		double nextTime = 0;
		int n = i + 1;
		
		if (n == this->FrameCount) // this handles condition when curr is last frame and next is first frame
		{
			n = 1;
			currTime = this->Frames[i].time;
			//nextTime = this->Frames[n].time;
			if (currTime <= this->TimeElapsed)
			{
				this->SetCurrentFrameTo(i);
				curr = this->GetCurrentKeyframe();
				next = this->GetKeyframe(n);
				break;
			}
		}
		else
		{
			currTime = this->Frames[i].time;
			nextTime = this->Frames[n].time;
			if (currTime <= this->TimeElapsed && this->TimeElapsed < nextTime)
			{
				this->SetCurrentFrameTo(i);
				curr = this->GetCurrentKeyframe();
				next = this->GetKeyframe(n);
				break;
			}
		}
	}

	// handle condition when curr is last frame and next is first frame
	if (curr && next)
	{
		double ratio = abs(this->TimeElapsed - curr->time) / abs(next->time - curr->time); // ratio
		tween->joints = curr->joints;
		tween->time = this->TimeElapsed;
		size_t jointCount = curr->joints.size();
		for (size_t i = 0; i < jointCount; ++i)
		{
			// interpolate rotation first bc setWorld changes position, fix that
			DirectX::XMVECTOR currRot = DirectX::XMQuaternionRotationMatrix(curr->joints[i].jointObject.GetWorldMatrix());
			DirectX::XMVECTOR nextRot = DirectX::XMQuaternionRotationMatrix(next->joints[i].jointObject.GetWorldMatrix());
			DirectX::XMMATRIX tweenMat = DirectX::XMMatrixRotationQuaternion(
				DirectX::XMQuaternionSlerp(currRot, nextRot, (float)ratio));
			// interpolate position
			DirectX::XMVECTOR lerpPos = DirectX::XMVectorLerp(curr->joints[i].jointObject.GetPositionVector(),
				next->joints[i].jointObject.GetPositionVector(), (float)ratio);
			tweenMat.r[3] = lerpPos;

			tween->joints[i].jointObject.SetWorld(tweenMat);
		}
	}

	return tween;
}

void Animation::Animation::StartPlayback()
{
	this->isPlaying = true;
}

void Animation::Animation::StopPlayback()
{
	this->isPlaying = false;
}

void Animation::Animation::FrameStepForward()
{
	if (this->CurrentFrame >= this->FrameCount - 1)
		this->CurrentFrame = 1;
	else
		++this->CurrentFrame;
}

void Animation::Animation::FrameStepBack()
{
	if (this->CurrentFrame <= 1)
		this->CurrentFrame = this->FrameCount - 1;
	else
		--this->CurrentFrame;
}

void Animation::Animation::ResetCurrentFrame()
{
	this->Timer.Stop();
	this->CurrentFrame = 0;
}

bool Animation::Animation::IsPlaying() const
{
	return this->isPlaying;
}
