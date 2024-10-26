#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <iostream>
#include "../Resources.h"
#include "../../Events/EventTypes.h"

using VIRTUAL_MACHINE_ID = size_t;

class VirtualMachine
{
    public:
        VirtualMachine() = default;
        VirtualMachine(VIRTUAL_MACHINE_ID vmID, EventTypes::Request&& request, PhysicalMachineResources resources) : 
            m_ID(vmID), m_Request(std::move(request)), m_CurrentlyUsedResources(resources) {};

        VIRTUAL_MACHINE_ID m_ID;
        EventTypes::Request m_Request;
        PhysicalMachineResources m_CurrentlyUsedResources;
};

#endif // VIRTUAL_MACHINE_H
