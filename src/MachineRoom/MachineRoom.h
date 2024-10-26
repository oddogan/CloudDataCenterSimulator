#ifndef MACHINEROOM_H
#define MACHINEROOM_H

#include "../Machines/PhysicalMachine/PhysicalMachine.h"
#include <vector>

class MachineRoom
{
    public:
        MachineRoom(const size_t expectedMachineCount = 0);
        ~MachineRoom();

        void DisplayInformation();
        std::vector<PhysicalMachine>& GetMachines();
        void AddMachine(const PhysicalMachine& machine);
        void RemoveMachine(const PHYSICAL_MACHINE_ID machineID);
        PhysicalMachine& GetMachine(const PHYSICAL_MACHINE_ID machineID);
        size_t GetMachineCount() const;

    private:
        std::vector<PhysicalMachine> m_Machines;
        size_t m_NumberOfTurnedOnMachines;
};

#endif
