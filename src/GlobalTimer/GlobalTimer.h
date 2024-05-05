#ifndef GLOBALTIMER_H
#define GLOBALTIMER_H

#include <chrono>
#include <thread>

class GlobalTimer
{
	public:
		static GlobalTimer& Instance()
		{
			static GlobalTimer timer;
			return timer;
		}

		void Start();
		void SleepFor(double duration_in_sec);
		double GetTime();
		void SetSpeedup(double Speedup_) { m_Speedup = Speedup_; };

	private:
		GlobalTimer() {};

		double m_Speedup;
		std::chrono::steady_clock::time_point initialTime;
};

#endif // GLOBALTIMER_H
