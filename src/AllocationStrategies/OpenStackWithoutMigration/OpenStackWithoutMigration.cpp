#include "OpenStackWithoutMigration.h"
#include <limits>
#include <stdexcept>

namespace AllocationStrategies
{

OpenStackWithoutMigration::OpenStackWithoutMigration()
{

}

OpenStackWithoutMigration::~OpenStackWithoutMigration()
{

}

std::vector<AllocationResult> OpenStackWithoutMigration::Run(std::vector<PhysicalMachine> &machines, std::vector<EventTypes::Request> &requests)
{
    std::vector<AllocationResult> allocationResults;
    std::pair<PhysicalMachine*, double> bestMatch;
    
    auto copyMachines = machines;
    auto copyRequests = requests;

    for (auto &request : copyRequests)
    {
        bestMatch.first = nullptr;
        bestMatch.second = std::numeric_limits<double>::max();

        for (auto& machine : copyMachines)
        {
            auto& availableResources = machine.GetAvailableResources();
            if (availableResources < request.m_RequestedResources) continue;

            auto& powerSpecs = machine.GetPowerSpecs();
            double powerIncrease = request.m_RequestedResources.CoreCount * powerSpecs.CPU;
            if (machine.IsPoweredOn() == false)
                powerIncrease += powerSpecs.Base;

            if (powerIncrease < bestMatch.second)
            {
                bestMatch.first = &machine;
                bestMatch.second = powerIncrease;
            }
        }

        if (bestMatch.first != nullptr)
        {
            bestMatch.first->AddVM(request);

            AllocationResult result;
            result.physicalMachine = bestMatch.first;
            result.request = &request;
            allocationResults.push_back(result);
        }
        else
        {
            std::cout << "[ERROR] VM " << request.m_RequestID << " cannot be placed anywhere!" << std::endl;
            throw std::runtime_error("Could not place the VM in any machine.");
        }
    }

    return allocationResults;
}

}
