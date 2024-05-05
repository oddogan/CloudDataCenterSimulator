#ifndef STATISTICSMANAGER_H
#define STATISTICSMANAGER_H

#include <vector>
#include <fstream>
#include <thread>
#include "../CloudDataCenter/CloudDataCenter.h"
#include "../EventDispatcher/EventDispatcher.h"
#include "../GlobalTimer/GlobalTimer.h"
#include <mutex>

class StatisticsManager
{
	public:
		StatisticsManager(CloudDataCenter& CloudDataCenter_, EventDispatcher& EventDispatcher_) :
			m_CloudDataCenter(CloudDataCenter_), m_EventDispatcher(EventDispatcher_) {};

		void StartCollecting(const char* cpOutputFilename);

	private:
		void CollectStatistics();

    	std::fstream m_fid;
		std::thread m_StatisticsThread;
		CloudDataCenter& m_CloudDataCenter;
		EventDispatcher& m_EventDispatcher;
};

#endif // STATISTICSMANAGER_H
