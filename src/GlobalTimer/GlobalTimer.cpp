#include "GlobalTimer.h"

GlobalTimer::GlobalTimer()
{
    m_InitialTime = 0;
    m_CurrentTime = 0;
}

void GlobalTimer::SleepFor(double duration_in_sec)
{
    std::this_thread::sleep_for(std::chrono::duration<double>(duration_in_sec));
}

double GlobalTimer::GetCurrentTime()
{
    return m_CurrentTime;
}

double GlobalTimer::GetInitialTime()
{
    return m_InitialTime;
}

void GlobalTimer::UpdateTime(double time)
{
    if (m_InitialTime == 0) m_InitialTime = time;
    m_CurrentTime = time;
}

void GlobalTimer::PrintTime()
{
    std::cout << "-- Current time is " << m_CurrentTime << " --\n";
}
