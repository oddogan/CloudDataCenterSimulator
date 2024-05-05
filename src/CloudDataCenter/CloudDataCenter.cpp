#include "CloudDataCenter.h"

CloudDataCenter::CloudDataCenter(EventDispatcher* EventDispatcher_) 
{
    /* System is not active yet */
    m_IsCurrentlyActive = false;

    /* Construct the machines in the cloud data center */
    ConstructMachines(500, 0);

    /* Connect to the event dispatcher */
    m_EventDispatcher = EventDispatcher_;

    /* Start the main control loop */
    // std::thread has a move assignment operator
    m_MainControllerThread = std::thread(&CloudDataCenter::MainController, this);
}

void CloudDataCenter::AddPhysicalMachine(const PhysicalMachine &machine_)
{
    m_PhysicalMachines.push_back(machine_);
}

void CloudDataCenter::RemovePhysicalMachine(const unsigned int uiMachineID_)
{
    for (auto it = m_PhysicalMachines.begin(); it != m_PhysicalMachines.end(); ++it)
    {
        if (it->GetMachineID() == uiMachineID_)
        {
            m_PhysicalMachines.erase(it);
            break;
        }
    }
}

PhysicalMachine& CloudDataCenter::GetPhysicalMachine(const unsigned int uiMachineID_)
{
    for (auto& machine : m_PhysicalMachines)
    {
        if (machine.GetMachineID() == uiMachineID_)
        {
            return machine;
        }
    }

    throw std::runtime_error("Physical machine not found.");
}

const std::vector<PhysicalMachine> &CloudDataCenter::GetPhysicalMachines() const
{
    return m_PhysicalMachines;
}

int CloudDataCenter::GetPhysicalMachineCount() const
{
    return m_PhysicalMachines.size();
}

void CloudDataCenter::DisplayStatus() const
{
    std::cout << "Cloud Data Center - Information" << std::endl;
    for (auto& machine : m_PhysicalMachines) machine.DisplayInfo();
}

void CloudDataCenter::MainController()
{
    m_IsCurrentlyActive = true;
    IEvent* event;

    while (true)
    {
        event = m_EventDispatcher->GetEvent();
        if (event == nullptr) continue;

        switch (event->GetEventType())
        {
            case EventType::SimulationEnd:
            {
                std::clog << "Simulation end event received!" << std::endl;
                m_IsCurrentlyActive = false;
                return;
            }
            case EventType::IncomingRequest:
            {
                EventTypes::Request* requestEvent = dynamic_cast<EventTypes::Request*>(event);
                std::clog << "Incoming request event (ID: " << requestEvent->m_RequestID << ") received!" << std::endl;

                m_RequestManager.AddRequestToBundle(requestEvent);

                if (m_RequestManager.BundleFilled() == true)
                {
                    ProcessRequests();
                    m_RequestManager.ClearBundle();
                };

                break;
            }
            case EventType::Exit:
            {
                EventTypes::Exit* exitEvent = dynamic_cast<EventTypes::Exit*>(event);
                std::clog << "Exit event (ID: " << exitEvent->m_RequestID << ") received!" << std::endl;

                PhysicalMachine* machine = m_RequestManager.GetVMOwnerPhysicalMachine(exitEvent->m_RequestID);
                if (machine)
                {
                    machine->RemoveVM(exitEvent->m_RequestID);
                    std::clog << "VM " << exitEvent->m_RequestID << " is removed." << std::endl;
                    // machine->DisplayInfo();
                }

                break;
            }
            case EventType::CPUUtilizationUpdate:
            {
                std::clog << "CPU Utilization Update event received!" << std::endl;
                break;
            }
            case EventType::VMDuplicationRemoval:
            {
                std::clog << "VM Duplication Removal event received!" << std::endl;
                break;
            }
            default:
            {
                std::cerr << "Unknown event received!" << std::endl;
                break;
            }
        }

        delete event;
    }
}

void CloudDataCenter::ProcessRequests()
{
    for (auto &request : m_RequestManager.GetRequestBundle())
    {
        bool isPlaced = false;
    
        for (auto &machine : m_PhysicalMachines)
        {
            if (machine.GetAvailableCoreCount() < request.m_Cores) continue;
            if (machine.GetAvailableRAM() < request.m_RAM) continue;
            if (machine.GetAvailableDisk() < request.m_Disk) continue;
            if (machine.GetAvailableBandwidth() < request.m_Bandwidth) continue;

            machine.AddVM(request.m_RequestID, request.m_Cores, request.m_RAM, request.m_Disk, request.m_Bandwidth);
            std::clog << "VM " << request.m_RequestID << " is started. It will stop at t: " << request.m_EventTime + request.m_RequestDuration << std::endl;
            // machine.DisplayInfo();
            m_RequestManager.SetVMOwnerPhysicalMachine(request.m_RequestID, machine);
            isPlaced = true;

            break;
        }

        if (isPlaced == false)
        {
            std::cerr << "VM " << request.m_RequestID << " cannot be placed anywhere!" << std::endl;
        }
    }
}

void CloudDataCenter::ConstructMachines(size_t iSmallMachineCount_, size_t iBigMachineCount_)
{
    /* Define the random number generator */
    static std::mt19937_64 rng(seed);

    /* Add small machines */
    {
        using namespace MachineTypes::SmallMachine;
        std::uniform_int_distribution<int> RandomCoreCount(CPU_Min, CPU_Max);
        std::uniform_real_distribution<double> RandomRAM(RAM_Min, RAM_Max);
        std::uniform_real_distribution<double> RandomDisk(Disk_Min, Disk_Max);
        std::uniform_real_distribution<double> RandomBandwidth(BW_Min, BW_Max);
        for (int i = 0; i < iSmallMachineCount_; ++i)
        {
            PhysicalMachine machine(RandomCoreCount(rng), RandomRAM(rng), RandomDisk(rng), RandomBandwidth(rng));
            AddPhysicalMachine(machine);
        }
    }

    /* Add big machines */
    {
        using namespace MachineTypes::BigMachine;
        std::uniform_int_distribution<int> RandomCoreCount(CPU_Min, CPU_Max);
        std::uniform_real_distribution<double> RandomRAM(RAM_Min, RAM_Max);
        std::uniform_real_distribution<double> RandomDisk(Disk_Min, Disk_Max);
        std::uniform_real_distribution<double> RandomBandwidth(BW_Min, BW_Max);
        for (int i = 0; i < iBigMachineCount_; ++i)
        {
            PhysicalMachine machine(RandomCoreCount(rng), RandomRAM(rng), RandomDisk(rng), RandomBandwidth(rng));
            AddPhysicalMachine(machine);
        }
    }
}
