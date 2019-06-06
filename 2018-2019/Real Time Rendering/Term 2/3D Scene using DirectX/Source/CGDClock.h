
//
// CGDClock.h
//

// Model a clock to track frames-per-second and seconds-per-frame for the host application

#pragma once


#include <string>


typedef long long gu_time_index;
typedef long long gu_time_interval;
typedef double gu_seconds;

class GUFrameCounter;


class CGDClock { // : public GUObject {

	// Clock status
	enum class clock_state : uint8_t { STOPPED = 0, START_DEFERRED, RUNNING };

	gu_time_index			baseTime = 0;
	gu_time_index			prevTimeIndex = 0;
	gu_time_index			currentTimeIndex = 0;

	gu_time_interval		deltaTime = 0;

	gu_time_index			stopTimeIndex = 0;
	gu_time_interval		totalStopTime = 0;

	clock_state				_clockState = clock_state::STOPPED;
	gu_seconds				deferredStartCounter = 0.0;

	GUFrameCounter			*frameCounter = nullptr;

	std::string				_clockName;


	//
	// Private interface
	//

	// Constructor - called internally by the CreateClock factory method defined above
	CGDClock(const std::string& clockName, const gu_seconds startDelay);


public:

	//
	// Public interface - Class methods
	//

	// Get the actual system clock time index
	static gu_time_index ActualTime();

	// Convert time interval measured in clock ticks into seconds (gu_seconds)
	static gu_seconds ConvertTimeIntervalToSeconds(gu_time_interval t);

	// Clock factory method
	static CGDClock* CreateClock(const std::string& clockName, const gu_seconds startDelay = 0.0);


	//
	// Public interface - Instance methods
	//

	// Destructor
	~CGDClock(void);

	// Update methods
	void start();
	void stop();
	void tick();
	void reset();

	// Query methods
	gu_seconds actualTimeElapsed() const;
	gu_seconds gameTimeElapsed() const;
	gu_seconds gameTimeDelta() const;

	// Return true if the clock is in the STOPPED or START_DEFERRED state, otherwise return false
	bool clockStopped() const;

	void reportTimingData() const;

	double framesPerSecond() const;
	double minimumFPS() const;
	double maximumFPS() const;
	gu_seconds averageFPS() const;
	gu_seconds secondsPerFrame() const;
	gu_seconds minimumSPF() const;
	gu_seconds maximumSPF() const;
	gu_seconds averageSPF() const;
};
