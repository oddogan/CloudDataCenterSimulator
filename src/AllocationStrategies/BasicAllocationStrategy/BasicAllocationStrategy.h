#ifndef BASICRESOURCEALLOCATOR_H
#define BASICRESOURCEALLOCATOR_H

#include "../IAllocationStrategy.h"

namespace AllocationStrategies
{

class BasicAllocationStrategy : public IAllocationStrategy
{
    public:
        BasicAllocationStrategy();
        ~BasicAllocationStrategy();

        std::vector<AllocationResult> Run(std::vector<PhysicalMachine> &machines, std::vector<EventTypes::Request> &requests) override;
};

}

#endif
