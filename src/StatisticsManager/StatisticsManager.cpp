#include "StatisticsManager.h"

void StatisticsManager::StartCollecting(const char* cpOutputFilename)
{
    m_fid.open(cpOutputFilename, std::ios::out | std::ios::binary);

    // std::thread has a move assignment operator
    m_StatisticsThread = std::thread(&StatisticsManager::CollectStatistics, this);
    m_StatisticsThread.detach();
}

void StatisticsManager::CollectStatistics()
{
    while (m_CloudDataCenter.IsCurrentlyActive())
    {        
        double time = GlobalTimer::Instance().GetTime();
        int count = 0;
        double sumCPU = 0;
        double sumRAM = 0;
        double sumDisk = 0;
        double sumBW = 0;

        for (auto &machine : m_CloudDataCenter.GetPhysicalMachines())
        {
            if (machine.IsPoweredOn())
            {
                count++;
                sumCPU += machine.GetUtilizationCPU();
                sumRAM += machine.GetUtilizationRAM();
                sumDisk += machine.GetUtilizationDisk();
                sumBW += machine.GetUtilizationBW();
            }
        }

        m_fid.write((char*) &time, sizeof(double));
        m_fid.write((char*) &count, sizeof(int));

        if (count == 0) 
        {
            count = 1;
            sumCPU = 1;
            sumRAM = 1;
            sumDisk = 1;
            sumBW = 1;
        }
        else
        {
            sumCPU /= count;
            sumRAM /= count;
            sumDisk /= count;
            sumBW /= count;
        }

        m_fid.write((char*) &sumCPU, sizeof(double));
        m_fid.write((char*) &sumRAM, sizeof(double));
        m_fid.write((char*) &sumDisk, sizeof(double));
        m_fid.write((char*) &sumBW, sizeof(double));

        GlobalTimer::Instance().SleepFor(1);
    }

    std::cout << "Saved to file." << std::endl;
    m_fid.close();
}
