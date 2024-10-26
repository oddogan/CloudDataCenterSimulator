#include "BasicAllocationStrategy.h"

namespace AllocationStrategies
{

BasicAllocationStrategy::BasicAllocationStrategy()
{

}

BasicAllocationStrategy::~BasicAllocationStrategy()
{

}

std::vector<AllocationResult> BasicAllocationStrategy::Run(std::vector<PhysicalMachine> &machines, std::vector<EventTypes::Request> &requests)
{
    std::vector<AllocationResult> allocationResults;
    auto copyMachines = machines;
    auto copyRequests = requests;

    for (auto &request : copyRequests)
    {
        bool isPlaced = false;

        for (auto &machine : copyMachines)
        {
            auto& availableResources = machine.GetAvailableResources();
            if (availableResources < request.m_RequestedResources) continue;

            machine.AddVM(request);
            
            AllocationResult result;
            result.physicalMachine = &machine;
            result.request = &request;
            allocationResults.push_back(result);
            isPlaced = true;

            break;
        }

        if (isPlaced == false)
        {
            std::cout << "[ERROR] VM " << request.m_RequestID << " cannot be placed anywhere!" << std::endl;
            throw std::runtime_error("Could not place the VM in any machine.");
        }
    }

    return allocationResults;
}

}
