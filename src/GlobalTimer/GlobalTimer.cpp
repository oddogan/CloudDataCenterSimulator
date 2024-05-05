#include "GlobalTimer.h"

void GlobalTimer::Start()
{
    initialTime = std::chrono::high_resolution_clock::now();
}

void GlobalTimer::SleepFor(double duration_in_sec)
{
    std::this_thread::sleep_for(std::chrono::duration<double>(duration_in_sec / m_Speedup));
}

double GlobalTimer::GetTime()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - initialTime).count() * 1e-9;

    return deltaTime * m_Speedup;
}
