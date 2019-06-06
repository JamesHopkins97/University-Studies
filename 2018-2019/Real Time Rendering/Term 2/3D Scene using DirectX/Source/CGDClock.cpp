
//
// CGDClock.cpp
//

#include <stdafx.h>
#include <CGDClock.h>
#include <Windows.h>
#include <iostream>

using namespace std;


static long long		performanceFrequency = 0;
static gu_seconds		timeRecip = 0.0;



// Private class to track frames-per-second (which varies non-linearly) and its inverse, seconds-per-frame (which varies linearly)
class GUFrameCounter {

private:

	int					_frame;
	gu_seconds			_fpsRefTimeIndex;
	unsigned long		_fpsCounts;
	double				_framesPerSecond, _minimumFPS, _maximumFPS, _averageFPS;
	gu_seconds			_secondsPerFrame, _minimumSPF, _maximumSPF, _averageSPF;


public:

	GUFrameCounter(gu_time_index baseTime = 0) {

		resetCounter(baseTime);
	}

	void resetCounter(gu_time_index resetTime = 0) {

		_frame = 0;

		_fpsRefTimeIndex = 0.0;

		_fpsCounts = 0;

		_framesPerSecond = _minimumFPS = _maximumFPS = _averageFPS = 0.0;
		_secondsPerFrame = _minimumSPF = _maximumSPF = _averageSPF = 0.0;
	}


	void updateFrameCounterForElaspsedTime(gu_seconds gameTimeElapsed) {

		_frame++;

		gu_seconds _time_delta = gameTimeElapsed - _fpsRefTimeIndex;

		if (_time_delta >= 1.0) {

			_framesPerSecond = (double)_frame / _time_delta;
			_secondsPerFrame = _time_delta / (double)_frame;

			if (_fpsCounts == 0) {

				// First iteration so initialise maximum, minimum and average fps and seconds per frame
				_minimumFPS = _maximumFPS = _averageFPS = _framesPerSecond;
				_minimumSPF = _maximumSPF = _averageSPF = _secondsPerFrame;

			}
			else {

				// Update maximum, minimum and average fps
				if (_framesPerSecond < _minimumFPS)
					_minimumFPS = _framesPerSecond;
				else if (_framesPerSecond > _maximumFPS)
					_maximumFPS = _framesPerSecond;

				_averageFPS += _framesPerSecond;

				// Update maximum, minimum and averse (milli)seconds per frame
				if (_secondsPerFrame < _minimumSPF)
					_minimumSPF = _secondsPerFrame;
				else if (_secondsPerFrame > _maximumSPF)
					_maximumSPF = _secondsPerFrame;

				_averageSPF += _secondsPerFrame;
			}

			// Reset frame counter for next iteration
			_frame = 0;

			// Note:  if a process takes significantly longer than 1 second, incrementing ref time index by 1.0 second means it lags behind the actual game time index so the next update call will track a low number of FPS (perhaps just 1).  So _fpsRefTimeIndex resets to the actual game time so we start tracking frames from this point.  This means the clock will be more precise for time complex operations (not likely in a game environment though)
			_fpsRefTimeIndex = gameTimeElapsed;

			_fpsCounts++;
		}

	}

	double framesPerSecond() const {
		
		return _framesPerSecond;
	}
	
	double minimumFPS() const {
		
		return _minimumFPS;
	}
	
	double maximumFPS() const {
		
		return _maximumFPS;
	}

	gu_seconds averageFPS() const {
		
		return ((gu_seconds)_averageFPS) / (gu_seconds)_fpsCounts;
	}

	gu_seconds secondsPerFrame() const {
		
		return _secondsPerFrame;
	}

	gu_seconds minimumSPF() const {
		
		return _minimumSPF;
	}

	gu_seconds maximumSPF() const {
		
		return _maximumSPF;
	}

	gu_seconds averageSPF() const {
		
		return _averageSPF / (gu_seconds)_fpsCounts;
	}

};



//
// CGDClock implementation
//



//
// Private interface
//

// Constructor - called internally by the CreateClock factory method defined above
CGDClock::CGDClock(const string& clockName, const gu_seconds startDelay) {

	frameCounter = new GUFrameCounter();

	if (frameCounter) {

		baseTime = CGDClock::ActualTime();
		prevTimeIndex = baseTime;

		deferredStartCounter = startDelay;

		if (deferredStartCounter > 0.0) {

			// Setup clock in deferred start state
			_clockState = clock_state::START_DEFERRED;
			stopTimeIndex = baseTime;

		}
		else {

			// Start clock
			_clockState = clock_state::RUNNING;
		}

		_clockName = string(clockName);
	}
}



//
// Public interface - Class methods
//


// Get the actual system clock time index
gu_time_index CGDClock::ActualTime() {

	gu_time_index t;

	QueryPerformanceCounter((LARGE_INTEGER*)&t);

	return t;
}


// Convert time interval measured in clock ticks into seconds (gu_seconds)
gu_seconds CGDClock::ConvertTimeIntervalToSeconds(gu_time_interval t) {

	return (performanceFrequency>0) ? (gu_seconds)t * timeRecip : 0.0;
}


// Clock factory method
CGDClock* CGDClock::CreateClock(const string& clockName, const gu_seconds startDelay) {

	static BOOL performanceFrequencyAcquired = FALSE;

	if (!performanceFrequencyAcquired) {

		QueryPerformanceFrequency((LARGE_INTEGER*)&performanceFrequency);

		if (performanceFrequency != 0) {

			// Valid high-performance counter present
			performanceFrequencyAcquired = TRUE;
			timeRecip = 1.0 / (gu_seconds)performanceFrequency;

		}
		else {

			// Cannot acquire performance frequency - return nullptr
			return nullptr;
		}
	}

	// Performance frequency acquired - create new clock
	return new CGDClock(clockName, startDelay);
}



//
// Public interface - Instance methods
//

// Destructor
CGDClock::~CGDClock(void) {

}


// Update methods

void CGDClock::start() {

	// Only allow clock to start if a valid performance frequency is present.  It is possible to start a clock if it's still in the START_DEFERRED state.
	if (clockStopped() && performanceFrequency != 0) {

		// Check if we're still in START_DEFERRED - if so report we're now starting the clock
		if (_clockState == clock_state::START_DEFERRED)
			cout << "Clock \"" << _clockName << "\" started\n";

		gu_time_index		restartTimeIndex = CGDClock::ActualTime();

		totalStopTime += restartTimeIndex - stopTimeIndex;
		prevTimeIndex = restartTimeIndex;

		_clockState = clock_state::RUNNING;
	}
}


// Stop an already running clock.  This method will not stop a clock in the START_DEFERRED state.
void CGDClock::stop() {

	if (!clockStopped()) {

		stopTimeIndex = CGDClock::ActualTime();

		_clockState = clock_state::STOPPED;
	}
}


void CGDClock::tick() {

	if (clockStopped()) {

		// Set game time delta to 0
		deltaTime = 0;

		if (_clockState == clock_state::START_DEFERRED) {

			// When running from start deferred, we want the actual time elapsed from baseTime ie. the time index when the clock was instantiated.  Once a clock state leaves START_DEFERRED it can never return to that state.
			if (actualTimeElapsed() > deferredStartCounter) {

				// Start the clock - deferred time interval has passed
				start();
			}
		}

		return;
	}

	currentTimeIndex = CGDClock::ActualTime();
	deltaTime = currentTimeIndex - prevTimeIndex;
	prevTimeIndex = currentTimeIndex;

	if (frameCounter)
		frameCounter->updateFrameCounterForElaspsedTime(CGDClock::ConvertTimeIntervalToSeconds((currentTimeIndex - baseTime) - totalStopTime));
}


void CGDClock::reset() {

	baseTime = CGDClock::ActualTime();

	prevTimeIndex = baseTime;

	deltaTime = 0;

	stopTimeIndex = 0;
	totalStopTime = 0;

	_clockState = clock_state::RUNNING;

	deferredStartCounter = 0.0;

	if (frameCounter)
		frameCounter->resetCounter();
}


// Query methods

gu_seconds CGDClock::actualTimeElapsed() const {

	return CGDClock::ConvertTimeIntervalToSeconds(CGDClock::ActualTime() - baseTime);
}


gu_seconds CGDClock::gameTimeElapsed() const {

	return CGDClock::ConvertTimeIntervalToSeconds((((clockStopped()) ? stopTimeIndex : CGDClock::ActualTime()) - baseTime) - totalStopTime);
}


gu_seconds CGDClock::gameTimeDelta() const {

	return CGDClock::ConvertTimeIntervalToSeconds(deltaTime);
}


// Return true if the clock is in the STOPPED or START_DEFERRED state, otherwise return false
bool CGDClock::clockStopped() const {

	return (_clockState == clock_state::STOPPED || _clockState == clock_state::START_DEFERRED);
}


void CGDClock::reportTimingData() const {

	if (frameCounter) {

		cout << "Frames per second...\n";
		cout << "Max FPS = " << frameCounter->maximumFPS() << endl;
		cout << "Min FPS = " << frameCounter->minimumFPS() << endl;
		cout << "Average FPS = " << frameCounter->averageFPS() << endl;

		cout << "\nSeconds per frame...\n";
		cout << "Max SPF = " << (frameCounter->maximumSPF()) << endl;
		cout << "Min SPF = " << (frameCounter->minimumSPF()) << endl;
		cout << "Average SPF = " << (frameCounter->averageSPF()) << endl;
	}
}


double CGDClock::framesPerSecond() const {

	return (frameCounter) ? frameCounter->framesPerSecond() : 0.0;
}


double CGDClock::minimumFPS() const {

	return (frameCounter) ? frameCounter->minimumFPS() : 0.0;
}


double CGDClock::maximumFPS() const {

	return (frameCounter) ? frameCounter->maximumFPS() : 0.0;
}


gu_seconds CGDClock::averageFPS() const {

	return (frameCounter) ? frameCounter->averageFPS() : 0.0;
}


gu_seconds CGDClock::secondsPerFrame() const {

	return (frameCounter) ? frameCounter->secondsPerFrame() : 0.0;
}


gu_seconds CGDClock::minimumSPF() const {

	return (frameCounter) ? frameCounter->minimumSPF() : 0.0;
}


gu_seconds CGDClock::maximumSPF() const {

	return (frameCounter) ? frameCounter->maximumSPF() : 0.0;
}


gu_seconds CGDClock::averageSPF() const {

	return (frameCounter) ? frameCounter->averageSPF() : 0.0;
}
