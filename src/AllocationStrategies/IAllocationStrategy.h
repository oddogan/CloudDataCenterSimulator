#ifndef IALLOCATIONSTRATEGY_H
#define IALLOCATIONSTRATEGY_H

#include <vector>
#include "../Machines/PhysicalMachine/PhysicalMachine.h"
#include "../Events/EventTypes.h"

struct AllocationResult
{
    PhysicalMachine* physicalMachine;
    EventTypes::Request* request;
};

class IAllocationStrategy
{
    public:
        virtual std::vector<AllocationResult> Run(std::vector<PhysicalMachine>& machines, std::vector<EventTypes::Request>& requests) = 0;
};

#endif
