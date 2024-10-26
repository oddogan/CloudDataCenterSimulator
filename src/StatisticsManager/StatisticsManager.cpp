#include "StatisticsManager.h"
#include <limits>

StatisticsManager::StatisticsManager()
{

}

void StatisticsManager::StartCollecting(const char* outputFilename)
{
    if (!m_fid.is_open())
    {
        m_Filename = outputFilename;
        m_fid.open(outputFilename, std::ios::out | std::ios::binary);

        if (!m_fid)
        {
            std::cout << "[ERROR] Could not open the file: " << outputFilename << std::endl;
            throw std::runtime_error("Could not open the statistics file!");
            return;
        }
    }
}

void StatisticsManager::StopCollecting()
{
    if (m_fid.is_open())
    {
        std::cout << "Saved " << m_DataCounter << " data to file " << m_Filename << std::endl;
        m_fid.close();
    }
}

void StatisticsManager::CollectStatistics(MachineRoom* machineRoom)
{
    if (m_fid.is_open() == false) return;

    double time = GlobalTimer::Instance().GetCurrentTime();
    int count = 0;
    double sumCPU = 0;
    double sumRAM = 0;
    double sumDisk = 0;
    double sumBW = 0;
    double maxUtilCPU = -1*std::numeric_limits<double>::max();
    double minUtilCPU = std::numeric_limits<double>::max();

    for (auto &machine : machineRoom->GetMachines())
    {
        if (machine.IsPoweredOn())
        {
            count++;

            maxUtilCPU = std::max(maxUtilCPU, machine.GetUtilizationCPU());
            minUtilCPU = std::min(minUtilCPU, machine.GetUtilizationCPU());

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
        maxUtilCPU = 1;
        minUtilCPU = 1;
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
    m_fid.write((char*) &minUtilCPU, sizeof(double));
    m_fid.write((char*) &maxUtilCPU, sizeof(double));

    m_DataCounter++;
}
