#include "AllocationStrategies/IAllocationStrategy.h"
#include "CloudDataCenter/CloudDataCenter.h"
#include "EventDispatcher/EventDispatcher.h"
#include "MachineRoom/MachineRoom.h"
#include "GlobalTimer/GlobalTimer.h"
#include "Machines/PhysicalMachine/PhysicalMachine.h"
#include "RequestManager/RequestManager.h"
#include "StatisticsManager/StatisticsManager.h"
#include "Machines/Machines.h"
#include "AllocationStrategies/BasicAllocationStrategy/BasicAllocationStrategy.h"
#include "AllocationStrategies/OpenStackWithoutMigration/OpenStackWithoutMigration.h"
#include "Utilities/util.h"
#include "../lib/ini.h"
#include <filesystem>
#include "AllocationStrategies/ILPStrategy/ILPStrategy.h"

void ConstructRandomMachines(MachineRoom& room, size_t iSmallMachineCount_, size_t iBigMachineCount_);

int main(int argc, char** argv)
{
    auto start = std::chrono::high_resolution_clock::now();

    std::cout << std::fixed;

    inih::INIReader* config;
    if (argc == 2)
    {
        config = new inih::INIReader(argv[1]);
    }
    else
    {
        config = new inih::INIReader("/Users/oddogan/MSc/CDC/CloudDataCenterSimulator/config/config.ini");
    }

    const auto& traceFilenames = config->GetVector<std::string>("Traces", "Filename");
    const auto& statisticsFilename = config->Get<std::string>("Statistics", "TargetFile");
    const auto& bigMachineCount = config->Get<size_t>("Machines", "BigMachineCount");
    const auto& smallMachineCount = config->Get<size_t>("Machines", "SmallMachineCount");
    const auto& strategyName = config->Get<std::string>("Strategy", "Name");
    const auto& bundleSize = config->Get<size_t>("Strategy", "BundleSize");

    for (auto& filename : traceFilenames)
    {
        std::cout << "[CONFIG] Trace file: " << filename << std::endl;
        std::cout << "[CONFIG] Trace size: " << std::filesystem::file_size(filename) / 1024.0 / 1024.0 << " MB" << std::endl;
    }
    std::cout << "[CONFIG] Statistics output file: " << statisticsFilename << std::endl;
    std::cout << "[CONFIG] Big Machine Count: " << bigMachineCount << std::endl;
    std::cout << "[CONFIG] Small Machine Count: " << smallMachineCount << std::endl;
    std::cout << "[CONFIG] Strategy Name: " << strategyName << std::endl;
    std::cout << "[CONFIG] Bundle size: " << bundleSize << std::endl;

    // Global Timer
    GlobalTimer& Timer = GlobalTimer::Instance();

    // The event dispatcher
    EventDispatcher Dispatcher;

    // Machine Room
    MachineRoom Room(smallMachineCount + bigMachineCount);
    ConstructRandomMachines(Room, smallMachineCount, bigMachineCount);

    // Request manager
    RequestManager ReqManager;
    ReqManager.SetBundleSize(bundleSize);

    // Choose the allocation strategy
    IAllocationStrategy* Strategy = nullptr;
    if (strategyName == "OpenStackWithoutMigration")
    {
        Strategy = new AllocationStrategies::OpenStackWithoutMigration();
        std::cout << "Strategy to be used: " << "OpenStackWithoutMigration" << std::endl;
    }
    else if (strategyName == "ILP")
    {
        Strategy = new AllocationStrategies::ILP();
        std::cout << "Strategy to be used: " << "ILP" << std::endl;
    }
    else
    {
        Strategy = new AllocationStrategies::BasicAllocationStrategy();
        std::cout << "Strategy to be used: " << "BasicAllocationStrategy" << std::endl;
    }

    // Constructing the Cloud Data Center (lol literally constructing)
    CloudDataCenter CDC;
    CDC.SetEventDispatcher(&Dispatcher);
    CDC.SetMachineRoom(&Room);
    CDC.SetAllocationStrategy(Strategy);
    CDC.SetRequestManager(&ReqManager);

    // Get the CDC information
    // CDC.DisplayStatus();

    // Statistics Manager will analyze the system
    StatisticsManager& Statistics = StatisticsManager::Instance();
    Statistics.StartCollecting(statisticsFilename.c_str());

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "[TIME] Initial setup took: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count()*1e-6 << " ms!" << std::endl;
    start = std::chrono::high_resolution_clock::now();

    // Start the simulation
    Dispatcher.EventReader(traceFilenames);
    std::cout << "All the requests have been added to the queue!" << std::endl;

    end = std::chrono::high_resolution_clock::now();
    std::cout << "[TIME] Reading the events took: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count()*1e-6 << " ms!" << std::endl;
    start = std::chrono::high_resolution_clock::now();

    // Start the CDC operation
    CDC.StartOperation();

    // Wait for data to finish
    CDC.WaitForFinish();
    std::cout << "Shutting the system down!" << std::endl;
    CDC.DisplayStatus();

    end = std::chrono::high_resolution_clock::now();
    std::cout << "[TIME] Simulation took: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count()*1e-6 << " ms!" << std::endl;

    // Save the collected statistics
    Statistics.StopCollecting();

    return 0;
}

void ConstructRandomMachines(MachineRoom& room, size_t iSmallMachineCount_, size_t iBigMachineCount_)
{
    PhysicalMachineResources resources;
    PhysicalMachinePowerSpecs power;

    /* Define the random number generator */
    static std::mt19937_64 rng(seed);

    /* Add big machines */
    {
        using namespace MachineTypes::BigMachine;
        std::uniform_int_distribution<int> RandomCoreCount(CPU_Min, CPU_Max);
        std::uniform_real_distribution<double> RandomRAM(RAM_Min, RAM_Max);
        std::uniform_real_distribution<double> RandomDisk(Disk_Min, Disk_Max);
        std::uniform_real_distribution<double> RandomBandwidth(BW_Min, BW_Max);
        std::uniform_real_distribution<double> RandomFPGA(FPGA_Min, FPGA_Max);

        for (int i = 0; i < iBigMachineCount_; ++i)
        {
            resources.CoreCount = RandomCoreCount(rng);
            resources.RAM = RandomRAM(rng);
            resources.Disk = RandomDisk(rng);
            resources.Bandwidth = RandomBandwidth(rng);
            resources.FPGA = RandomFPGA(rng);
            power.Base = 10;
            power.CPU = 10;
            PhysicalMachine machine(resources, power);
            room.AddMachine(machine);
        }
    }

    /* Add small machines */
    {
        using namespace MachineTypes::SmallMachine;
        std::uniform_int_distribution<int> RandomCoreCount(CPU_Min, CPU_Max);
        std::uniform_real_distribution<double> RandomRAM(RAM_Min, RAM_Max);
        std::uniform_real_distribution<double> RandomDisk(Disk_Min, Disk_Max);
        std::uniform_real_distribution<double> RandomBandwidth(BW_Min, BW_Max);
        std::uniform_real_distribution<double> RandomFPGA(FPGA_Min, FPGA_Max);
        for (int i = 0; i < iSmallMachineCount_; ++i)
        {
            resources.CoreCount = RandomCoreCount(rng);
            resources.RAM = RandomRAM(rng);
            resources.Disk = RandomDisk(rng);
            resources.Bandwidth = RandomBandwidth(rng);
            resources.FPGA = RandomFPGA(rng);
            power.Base = 10;
            power.CPU = 10;
            PhysicalMachine machine(resources, power);
            room.AddMachine(machine);
        }
    }
}
