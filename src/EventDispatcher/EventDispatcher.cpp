#include "EventDispatcher.h"

EventDispatcher::EventDispatcher(const char *cpTraceFilename_, size_t szNumFiles_)
{
    m_TraceDatabasePrefix = cpTraceFilename_;
    m_TraceNumFiles = szNumFiles_;
}

EventDispatcher::~EventDispatcher()
{
}

void EventDispatcher::Start()
{
    // std::thread has a move assignment operator
    m_DispatcherThread = std::thread(&EventDispatcher::EventReader, this);
}

void EventDispatcher::EventReader()
{
    char cpTraceFile[256];
    std::ifstream fid;
    std::string line;
    std::vector<std::string> values;

    for (int iFileInd = 0; iFileInd < m_TraceNumFiles; ++iFileInd)
    {
        snprintf(cpTraceFile, 256, "%s%d.txt", m_TraceDatabasePrefix.c_str(), iFileInd);

        fid.open(cpTraceFile);

        std::getline(fid, line); // Ignore the header line

        while (std::getline(fid, line))
        {
            std::lock_guard<std::mutex> lock(m_mtxProtectQueue);

            values = ParseStringViaDelimiter(line, ",");

            EventTypes::Request* event = new EventTypes::Request();
            event->m_EventTime = atoi(values.at(2).c_str());
            event->m_RequestID = atoi(values.at(0).c_str());
            event->m_RequestDuration = atoi(values.at(3).c_str());
            event->m_Cores = atoi(values.at(4).c_str());
            event->m_RAM = atoi(values.at(6).c_str());
            event->m_Bandwidth = atoi(values.at(8).c_str());
            event->m_Disk = atoi(values.at(7).c_str());
            m_EventQueue.push(event); 

            /* Create the machine exit event for future */
            EventTypes::Exit* exitEvent = new EventTypes::Exit();
            exitEvent->m_EventTime = event->m_EventTime + event->m_RequestDuration;
            exitEvent->m_RequestID = event->m_RequestID;
            m_EventQueue.push(exitEvent);

            // Notify the request processor about the new event
            m_condVariable.notify_one();
            // TODO: Just for testing
            //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        fid.close();
    }

    /* Stop the cloud data center simulation when all the queue is consumed */
    while (!m_EventQueue.empty()) {}

    EventTypes::SimulationEnd* endEvent = new EventTypes::SimulationEnd();
    endEvent->m_EventTime = GlobalTimer::Instance().GetTime() + 60*60*24;
    m_EventQueue.push(endEvent);
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

                case EventType::IncomingRequest:
                {
                    EventTypes::Request* event = new EventTypes::Request();
                    event->m_EventTime = time;
                    event->m_RequestID = reqCounter++;
                    event->m_RequestDuration = duration;
                    event->m_Cores = 8;
                    event->m_RAM = 16;
                    event->m_Bandwidth = 5000;
                    event->m_Disk = 10000;
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

    double currentTime = GlobalTimer::Instance().GetTime();
    IEvent* returnEvent = m_EventQueue.top();

    /* If time has not come yet (except SimulationEnd event) */
    if (returnEvent->GetEventType() != EventType::SimulationEnd)
    {
        if (returnEvent->m_EventTime > currentTime)
        {
            return nullptr;
        }
    }

    m_EventQueue.pop();
    return returnEvent;
}

void EventDispatcher::AddEvent(IEvent *event_)
{
    std::unique_lock<std::mutex> lock(m_mtxProtectQueue);

    m_EventQueue.push(event_);
}
