#ifndef ILPSTRATEGY_H
#define ILPSTRATEGY_H

#include "../IAllocationStrategy.h"

namespace AllocationStrategies
{

class ILP : public IAllocationStrategy
{
    public:
        ILP();
        ~ILP();

        std::vector<AllocationResult> Run(std::vector<PhysicalMachine> &machines, std::vector<EventTypes::Request> &requests) override;

        void SetMigrationCost(int cost);
        void SetTau(double tau);

    private:
        int m_migrationCost;
        int m_maximumRequestsInPM;
        double m_Tau;
};

}


#endif