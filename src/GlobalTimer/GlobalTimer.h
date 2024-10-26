#ifndef GLOBALTIMER_H
#define GLOBALTIMER_H

#include <chrono>
#include <thread>
#include <iostream>

class GlobalTimer
{
	public:
		static GlobalTimer& Instance()
		{
			static GlobalTimer timer;
			return timer;
		}

		double GetCurrentTime();
		double GetInitialTime();
		void UpdateTime(double time);
		void SleepFor(double duration_in_sec);
		void PrintTime();

	private:
		GlobalTimer();

		double m_InitialTime;
		double m_CurrentTime;
};

#endif // GLOBALTIMER_H
