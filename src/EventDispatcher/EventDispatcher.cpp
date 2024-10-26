#include "EventDispatcher.h"
#include <limits>

EventDispatcher::EventDispatcher()
{
}

EventDispatcher::~EventDispatcher()
{
}

void EventDispatcher::EventReader(const std::vector<std::string>& traceFilenames)
{
    std::ifstream fid;
    std::string line;
    std::vector<std::string> values;
    size_t counter = 0;
    double latestTime = std::numeric_limits<double>::max();

    for (auto& filename : traceFilenames)
    {
        fid.open(filename);

        if (!fid)
        {
            std::cout << "[ERROR] Could not open the file: " << filename << std::endl;
            throw std::runtime_error("Could not open the trace file!");
            return;
        }

        std::getline(fid, line); // Ignore the header line

        while (std::getline(fid, line))
        {
            // std::lock_guard<std::mutex> lock(m_mtxProtectQueue);

            values = ParseStringViaDelimiter(line, ",");

            EventTypes::Request* event = new EventTypes::Request();
            event->m_EventTime = atof(values.at(2).c_str());
            event->m_RequestID = atoi(values.at(0).c_str());
            event->m_RequestDuration = atof(values.at(3).c_str());
            event->m_RequestedResources.CoreCount = atoi(values.at(4).c_str());
            event->m_RequestedResources.RAM = atof(values.at(6).c_str());
            event->m_RequestedResources.Bandwidth = atof(values.at(8).c_str());
            event->m_RequestedResources.Disk = atof(values.at(7).c_str());
            for (auto i = 10; i < values.size(); ++i)
                event->m_utilizationUpdateValues.push_back(atof(values.at(i).c_str()));
            event->m_CurrentUtilization = event->m_utilizationUpdateValues.at(0);
            event->m_UtilizationUpdateCounter = 1;
            AddEvent(event);

            #if 0
            /* Create the machine exit event for future */
            EventTypes::Exit* exitEvent = new EventTypes::Exit();
            exitEvent->m_EventTime = event->m_EventTime + event->m_RequestDuration;
            exitEvent->m_RequestID = event->m_RequestID;
            AddEvent(exitEvent);
            latestTime = std::max(latestTime, exitEvent->m_EventTime);
            #endif

            counter++;

            // Notify the request processor about the new event
            // m_condVariable.notify_one();
        }

        fid.close();
    }

    std::cout << "\rRequests read: " << counter << std::endl;

    /* Stop the cloud data center simulation after all the queue is consumed */
    EventTypes::SimulationEnd* endEvent = new EventTypes::SimulationEnd();
    endEvent->m_EventTime = latestTime;
    AddEvent(endEvent);
    m_condVariable.notify_one();
}

std::vector<std::string> EventDispatcher::ParseStringViaDelimiter(std::string line_, const char* delimiter_)
{
    std::vector<std::string> result;
    int pos = 0;

    while(pos < line_.size()){
        pos = line_.find(delimiter_);
        result.push_back(line_.substr(0,pos));
        line_.erase(0, pos + strlen(delimiter_));
    }

    return result;
}

void EventDispatcher::RandomEventGenerator()
{
    unsigned int reqCounter = 0;
    std::vector<unsigned int> activeVMs;
    while (true)
    {
        {
            std::lock_guard<std::mutex> lock(m_mtxProtectQueue);
            // TODO: Insert new events to the event queue
            EventType type = static_cast<EventType>(std::rand() % 5 - 1);
            double time = std::rand() % 10000;
            double duration = std::rand() % 1000 + 1000;

            switch (type)
            {
                case EventType::SimulationEnd:
                {
                    EventTypes::SimulationEnd* event = new EventTypes::SimulationEnd();
                    event->m_EventTime = time;
                    m_EventQueue.push(event);
                    break;
                }

                case EventType::Request:
                {
                    EventTypes::Request* event = new EventTypes::Request();
                    event->m_EventTime = time;
                    event->m_RequestID = reqCounter++;
                    event->m_RequestDuration = duration;
                    event->m_RequestedResources.CoreCount = 8;
                    event->m_RequestedResources.RAM = 16;
                    event->m_RequestedResources.Disk = 10000;
                    event->m_RequestedResources.Bandwidth = 5000;
                    m_EventQueue.push(event);

                    /* Create the machine exit event for future */
                    EventTypes::Exit* exitEvent = new EventTypes::Exit();
                    exitEvent->m_EventTime = event->m_EventTime + event->m_RequestDuration;
                    exitEvent->m_RequestID = event->m_RequestID;
                    m_EventQueue.push(exitEvent);

                    break;
                }

                case EventType::Exit:
                {
                    if (activeVMs.size() == 0) continue;

                    EventTypes::Exit* event = new EventTypes::Exit();
                    event->m_EventTime = time;
                    size_t VMIndex = std::rand() % activeVMs.size();
                    event->m_RequestID = activeVMs[VMIndex];
                    m_EventQueue.push(event);
                    activeVMs.erase(activeVMs.begin() + VMIndex);
                    break;
                }

                case EventType::CPUUtilizationUpdate:
                {
                    EventTypes::CPUUtilizationUpdate* event = new EventTypes::CPUUtilizationUpdate();
                    event->m_EventTime = time;
                    m_EventQueue.push(event);
                    break;
                }

                case EventType::VMDuplicationRemoval:
                {
                    EventTypes::VMDuplicationRemoval* event = new EventTypes::VMDuplicationRemoval();
                    event->m_EventTime = time;
                    m_EventQueue.push(event);
                    break;
                }

                default:
                {
                    break;
                }
            }
        }

        // Notify the request processor about the new event
        m_condVariable.notify_one();

        // TODO: Just for testing
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

IEvent* EventDispatcher::GetEvent()
{
    std::unique_lock<std::mutex> lock(m_mtxProtectQueue);

    m_condVariable.wait(lock, [&]{ return !m_EventQueue.empty(); });

    IEvent* returnEvent = m_EventQueue.top();
    m_EventQueue.pop();

    return returnEvent;
}

void EventDispatcher::AddEvent(IEvent *event_)
{
    // std::unique_lock<std::mutex> lock(m_mtxProtectQueue);

    m_EventQueue.push(event_);
}
