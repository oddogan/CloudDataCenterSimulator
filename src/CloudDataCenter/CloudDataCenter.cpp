#include "CloudDataCenter.h"

CloudDataCenter::CloudDataCenter()
{
    /* System is not active yet */
    m_IsCurrentlyActive = false;
}

bool CloudDataCenter::StartOperation()
{
    if (m_EventDispatcher == nullptr)
    {
        std::cout << "[ERROR] Event dispatcher has not been connected to the CDC yet!\n";
        throw std::runtime_error("Event dispatcher has not been connected to the CDC yet!");
        return false;
    }

    if (m_MachineRoom == nullptr)
    {
        std::cout << "[ERROR] Machine room has not been connected to the CDC yet!\n";
        throw std::runtime_error("Machine room has not been connected to the CDC yet!");
        return false;
    }

    if (m_RequestManager == nullptr)
    {
        std::cout << "[ERROR] Request manager has not been connected to the CDC yet!\n";
        throw std::runtime_error("Request manager has not been connected to the CDC yet!");
        return false;
    }

    /* Start the main control loop */
    // std::thread has a move assignment operator
    m_MainControllerThread = std::thread(&CloudDataCenter::MainController, this);
    return true;
}

void CloudDataCenter::SetEventDispatcher(EventDispatcher *eventDispatcher)
{
    m_EventDispatcher = eventDispatcher;
}

void CloudDataCenter::SetMachineRoom(MachineRoom *machineRoom)
{
    m_MachineRoom = machineRoom;
}

void CloudDataCenter::SetAllocationStrategy(IAllocationStrategy *strategy)
{
    m_AllocationStrategy = strategy;
}

void CloudDataCenter::SetRequestManager(RequestManager *requestManager)
{
    m_RequestManager = requestManager;
}

EventDispatcher* CloudDataCenter::GetEventDispatcher()
{
    return m_EventDispatcher;
}

MachineRoom* CloudDataCenter::GetMachineRoom()
{
    return m_MachineRoom;
}

IAllocationStrategy* CloudDataCenter::GetAllocationStrategy()
{
    return m_AllocationStrategy;
}

RequestManager* CloudDataCenter::GetRequestManager()
{
    return m_RequestManager;
}

void CloudDataCenter::DisplayStatus() const
{
    std::cout << "----- Cloud Data Center | Information -----" << std::endl;
    m_MachineRoom->DisplayInformation();
    std::cout << "-------------------------------------------" << std::endl;
}

void CloudDataCenter::MainController()
{
    m_IsCurrentlyActive = true;

    while (true)
    {
        bool bCollectStatistics = false;
        IEvent* event = m_EventDispatcher->GetEvent();
        EventType type = event->GetEventType();
        GlobalTimer::Instance().UpdateTime(event->m_EventTime);

        switch (type)
        {
            case EventType::SimulationEnd:
            {
                std::cout << "Simulation end event received!" << std::endl;
                bCollectStatistics = true;
                m_IsCurrentlyActive = false;
                break;
            }
            case EventType::Request:
            {
                EventTypes::Request* requestEvent = dynamic_cast<EventTypes::Request*>(event);
                /*
                std::cout << "Incoming request event (ID: " << requestEvent->m_RequestID << ") received!" << std::endl;
                std::cout << "Requested resources: " << requestEvent->m_RequestedResources.CoreCount << " cores, "
                    << requestEvent->m_RequestedResources.RAM << " GB RAM, "
                    << requestEvent->m_RequestedResources.Disk << " GB Disk, "
                    << requestEvent->m_RequestedResources.Bandwidth << " Mbps Bandwidth" << std::endl;
                    */

                m_RequestManager->AddRequestToBundle(requestEvent);

                if (m_RequestManager->BundleFilled() == true)
                {
                    ProcessRequests();
                    m_RequestManager->ClearBundle();
                    bCollectStatistics = true;
                };

                break;
            }
            case EventType::Exit:
            {
                EventTypes::Exit* exitEvent = dynamic_cast<EventTypes::Exit*>(event);
                std::cout << "Exit event (ID: " << exitEvent->m_RequestID << ") received!" << std::endl;

                PhysicalMachine* machine = m_RequestManager->GetVMOwnerPhysicalMachine(exitEvent->m_RequestID);
                if (machine)
                {
                    machine->RemoveVM(exitEvent->m_RequestID);
                    std::cout << "VM (ID:" << exitEvent->m_RequestID << ") is removed." << std::endl;
                    machine->DisplayInfo();
                }
                else
                {
                    break;
                    throw std::runtime_error("Cannot find VM in any physical machines, it has not been placed anywhere!");
                }

                bCollectStatistics = true;
                break;
            }
            case EventType::CPUUtilizationUpdate:
            {
                EventTypes::CPUUtilizationUpdate* updateEvent = dynamic_cast<EventTypes::CPUUtilizationUpdate*>(event);
                //std::cout << "CPU Utilization Update event (ID: " << updateEvent->m_RequestID << " received!" << std::endl;

                auto PM = m_RequestManager->GetVMOwnerPhysicalMachine(updateEvent->m_RequestID);
                auto VM = PM->GetVM(updateEvent->m_RequestID);
                PM->UpdateVMUtilization(updateEvent->m_RequestID, updateEvent->m_UtilizationValue);

                // SLAV may occur, migrate the VM
                if (PM->GetUtilizationCPU() > 1)
                {
                    ProcessRequests();
                    m_RequestManager->ClearBundle();
                }

                /* Create the update event */
                if (VM->m_Request.m_UtilizationUpdateCounter+2 < VM->m_Request.m_utilizationUpdateValues.size())
                {
                    EventTypes::CPUUtilizationUpdate* nextUpdateEvent = new EventTypes::CPUUtilizationUpdate();
                    nextUpdateEvent->m_RequestID = VM->m_Request.m_RequestID;
                    nextUpdateEvent->m_EventTime = GlobalTimer::Instance().GetCurrentTime() + 300;
                    nextUpdateEvent->m_UtilizationValue = VM->m_Request.m_utilizationUpdateValues.at(VM->m_Request.m_UtilizationUpdateCounter++);
                    m_EventDispatcher->AddEvent(nextUpdateEvent);
                }

                bCollectStatistics = true;
                break;
            }
            case EventType::VMDuplicationRemoval:
            {
                std::cout << "VM Duplication Removal event received!" << std::endl;
                break;
            }
            default:
            {
                std::cout << "[ERROR] Unknown event received!" << std::endl;
                break;
            }
        }

        if (bCollectStatistics) StatisticsManager::Instance().CollectStatistics(m_MachineRoom);
        GlobalTimer::Instance().PrintTime();

        delete event;

        if (type == EventType::SimulationEnd) return;
    }
}

void CloudDataCenter::ProcessRequests()
{
    auto allocationResults = m_AllocationStrategy->Run(m_MachineRoom->GetMachines(), m_RequestManager->GetRequestBundle());

    for (auto& result : allocationResults)
    {
        result.physicalMachine->AddVM(*result.request);
        m_RequestManager->SetVMOwnerPhysicalMachine(result.request->m_RequestID, *result.physicalMachine);

        /* Create the machine exit event for future */
        EventTypes::Exit* exitEvent = new EventTypes::Exit();
        exitEvent->m_EventTime = GlobalTimer::Instance().GetCurrentTime() + result.request->m_RequestDuration;
        exitEvent->m_RequestID = result.request->m_RequestID;
        m_EventDispatcher->AddEvent(exitEvent);
        std::cout << "VM " << result.request->m_RequestID << " is started at PM " << result.physicalMachine->GetMachineID() << ". It will stop at t: " << exitEvent->m_EventTime << std::endl;

        /* Create the update event */
        if (result.request->m_UtilizationUpdateCounter+2 < result.request->m_utilizationUpdateValues.size())
        {
            EventTypes::CPUUtilizationUpdate* updateEvent = new EventTypes::CPUUtilizationUpdate();
            updateEvent->m_RequestID = result.request->m_RequestID;
            updateEvent->m_EventTime = GlobalTimer::Instance().GetCurrentTime() + 300;
            updateEvent->m_UtilizationValue = result.request->m_utilizationUpdateValues.at(result.request->m_UtilizationUpdateCounter++);
            m_EventDispatcher->AddEvent(updateEvent);
            std::cout << "VM " << result.request->m_RequestID << " at PM " << result.physicalMachine->GetMachineID() << " will be updated at " << updateEvent->m_EventTime << std::endl;
        }
    }
}
