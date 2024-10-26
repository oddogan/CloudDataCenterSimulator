#include "MachineRoom.h"
#include <stdexcept>
#include <vector>

MachineRoom::MachineRoom(const size_t expectedMachineCount)
{
    if (expectedMachineCount)
    {
        m_Machines.reserve(expectedMachineCount);
    }
}

MachineRoom::~MachineRoom()
{
}

void MachineRoom::DisplayInformation()
{
    for (auto &machine : m_Machines)
    {
        machine.DisplayInfo();
    }
}

std::vector<PhysicalMachine>& MachineRoom::GetMachines()
{
    return m_Machines;
}

void MachineRoom::AddMachine(const PhysicalMachine &machine)
{
    m_Machines.push_back(machine);
}

void MachineRoom::RemoveMachine(const PHYSICAL_MACHINE_ID machineID)
{
    for (auto it = m_Machines.begin(); it != m_Machines.end(); ++it)
    {
        if (it->GetMachineID() == machineID)
        {
            m_Machines.erase(it);
            return;
        }
    }

    throw std::runtime_error("Cannot remove the physical machine!");
}

PhysicalMachine& MachineRoom::GetMachine(const PHYSICAL_MACHINE_ID machineID)
{
    for (auto& machine : m_Machines)
    {
        if (machine.GetMachineID() == machineID)
        {
            return machine;
        }
    }

    throw std::runtime_error("Physical machine not found.");
}

size_t MachineRoom::GetMachineCount() const
{
    return m_Machines.size();
}
