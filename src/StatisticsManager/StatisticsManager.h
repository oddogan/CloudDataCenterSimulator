#ifndef STATISTICSMANAGER_H
#define STATISTICSMANAGER_H

#include <fstream>
#include <thread>
#include "../MachineRoom/MachineRoom.h"
#include "../GlobalTimer/GlobalTimer.h"

class StatisticsManager
{
	public:
    	static StatisticsManager& Instance()
  		{
 			static StatisticsManager manager;
 			return manager;
  		}

        void CollectStatistics(MachineRoom* machineRoom);
		void StartCollecting(const char* outputFilename);
		void StopCollecting();

	private:
	    StatisticsManager();

    	std::fstream m_fid;
		std::thread m_StatisticsThread;
		std::string m_Filename;
		size_t m_DataCounter;
};

#endif // STATISTICSMANAGER_H
