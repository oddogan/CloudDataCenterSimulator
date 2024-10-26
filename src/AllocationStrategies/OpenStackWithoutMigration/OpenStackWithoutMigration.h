#ifndef OPENSTACK_WITHOUT_MIGRATION_H
#define OPENSTACK_WITHOUT_MIGRATION_H

#include "../IAllocationStrategy.h"

namespace AllocationStrategies
{

class OpenStackWithoutMigration : public IAllocationStrategy
{
    public:
        OpenStackWithoutMigration();
        ~OpenStackWithoutMigration();

        std::vector<AllocationResult> Run(std::vector<PhysicalMachine> &machines, std::vector<EventTypes::Request> &requests) override;
};

}

#endif
