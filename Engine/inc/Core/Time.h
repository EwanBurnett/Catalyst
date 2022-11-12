//Time Class - Ewan Burnett 2021
//The Time class simulates real world style time within the application. 
//Each instance of Time is local, though most gameobjects will use global time.
#pragma once
#include "../Framework.h"
#include <chrono>

class Time {
public:
	Time();

	//Returns time in seconds
	float TotalTime() const;
	//Returns the Interval between frames. 
	float DeltaTime() const;

	//Returns the last time Reset() was called. 
	float BaseTime() const;

	//Changes the timescale of the application
	void SetTimeScale(float timeScale);

	void Start();	//Starting from a pause
	void Reset();	//before message loop
	void Stop();	//when game is paused
	void Tick();	//once per frame

	bool IsPaused();

private:
	double mSecondsPerCount;
	double mDeltaTime;
	double mTimeScale;		//Timescale of the application. 1 = normal, 0.5 = half speed, etc.,

	__int64 mBaseTime;		//The time when the application started or when Reset() was last called)
	__int64 mStopTime;		//Time when the application was paused
	__int64 mPausedTime;	//How long the application has been paused for
	__int64 mPrevTime;		//Time of the previous frame
	__int64 mCurrentTime;	//The time of the application

	bool mIsStopped;		//Whether the time is stopped or not
};

inline Time::Time()
	: mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0), mCurrentTime(0),
	mPausedTime(0), mPrevTime(0), mStopTime(0), mIsStopped(false), mTimeScale(1)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;

	//Set the base time of this timer after construction is complete
	Reset();
}

inline float Time::TotalTime() const
{
	//Returns the total time in the scope of the timer. Does not include paused time.
	if (mIsStopped) {
		return((float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount * mTimeScale));
	}
	else {
		return((float)(((mCurrentTime - mPausedTime) - mBaseTime) * mSecondsPerCount * mTimeScale));
	}

}

inline float Time::DeltaTime() const
{
	return (float)(mDeltaTime * mTimeScale);
}

inline float Time::BaseTime() const
{
	return (float)mBaseTime;
}

inline void Time::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mIsStopped = false;
}

inline void Time::SetTimeScale(float timeScale)
{
	if (timeScale > 0) {
		mTimeScale = timeScale;
	}
}

inline void Time::Start()
{
	__int64 startTime = 0;

	//Starting from a pause
	if (mIsStopped) {
		mPausedTime += (startTime - mStopTime);

		mPrevTime = startTime;

		mStopTime = 0;
		mIsStopped = false;
	}
}

inline void Time::Stop()
{
	if (!mIsStopped) {
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		//Save the time we paused at, and pause.
		mPausedTime = currTime;
		mIsStopped = true;
	}
}

inline void Time::Tick()
{
	//If the application is paused, do nothing.
	if (mIsStopped) {
		mDeltaTime = 0.0;
		return;
	}

	//Get the time this frame
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrentTime = currTime;

	//Calculate the time difference between this frame and the last
	mDeltaTime = (mCurrentTime - mPrevTime) * mSecondsPerCount;

	//Prepare for the next frame
	mPrevTime = mCurrentTime;

	//Force non-negative deltatime
	if (mDeltaTime < 0) {
		mDeltaTime = 0.0;
	}
}

inline bool Time::IsPaused()
{
	return mIsStopped;
}


namespace Timing
{

	/**
	 * \brief Represents the time interval between frames.
	 */
	struct Time
	{
		Time() : seconds(0), milliseconds(0), microseconds(0), frameRate(0), frame(-1)
		{}
		double seconds, milliseconds, microseconds, frameRate;

		std::chrono::high_resolution_clock::time_point t;
		int frame;
	};

	/**
	 * \brief Advances the timer.
	 * \param time the output time
	 */
	inline void tick(Time& time)
	{
		std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> interval = end.time_since_epoch() - time.t.time_since_epoch();

		time.seconds = interval.count();
		time.milliseconds = time.seconds * 1000.0;
		time.microseconds = time.milliseconds * 1000.0;
		time.frameRate = 1.0 / time.seconds;

		time.t = end;
		++time.frame;
	}

	inline std::time_t Now()
	{
		return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	}
}