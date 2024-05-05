#include "CloudDataCenter/CloudDataCenter.h"
#include "EventDispatcher/EventDispatcher.h"
#include "GlobalTimer/GlobalTimer.h"
#include "StatisticsManager/StatisticsManager.h"

int main(int argc, char** argv)
{
    auto start = std::chrono::high_resolution_clock::now();

    // Global Timer
    GlobalTimer& Timer = GlobalTimer::Instance();
    Timer.SetSpeedup(1e9);
    Timer.Start();

    // The event dispatcher
    EventDispatcher Dispatcher("/Users/oddogan/MSc/CDC/trace_deneme_", 1);

    // Constructing the Cloud Data Center (lol literally constructing)
    CloudDataCenter CDC(&Dispatcher);

    // Get the CDC information
    CDC.DisplayStatus();

    // Statistics Manager will analyze the system
    StatisticsManager Statistics(CDC, Dispatcher);
    Statistics.StartCollecting("/Users/oddogan/MSc/CDC/log.bin");

    // Start the simulation
    Dispatcher.Start();

    // Wait for data to finish
    Dispatcher.WaitForFinish();
    CDC.WaitForFinish();
    std::clog << "Shutting the system down!" << std::endl;

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "The simulation took: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count()*1e-6 << " ms!" << std::endl;

    return 0;
}