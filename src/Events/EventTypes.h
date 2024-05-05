#ifndef EVENT_TYPES_H
#define EVENT_TYPES_H

#include "Event.h"

namespace EventTypes
{
    class Request : public IEvent
    {
        public:
            Request() {} ;
            Request(double RequestTime_, double RequestDuration_, unsigned int RequestID_, unsigned int Cores_, double RAM_, double Disk_, double Bandwidth_)
                : IEvent(RequestTime_), m_RequestDuration(RequestDuration_), m_RequestID(RequestID_), m_Cores(Cores_), m_RAM(RAM_), m_Disk(Disk_), m_Bandwidth(Bandwidth_) {}
            virtual ~Request() {};
            EventType GetEventType() { return EventType::IncomingRequest; };
            double m_RequestDuration;
            unsigned int m_RequestID;
            unsigned int m_Cores;
            double m_RAM;
            double m_Disk;
            double m_Bandwidth;
    };

    class SimulationEnd : public IEvent
    {
        EventType GetEventType() { return EventType::SimulationEnd; };
    };

    class Exit : public IEvent
    {
        public:
            EventType GetEventType() { return EventType::Exit; };
            unsigned int m_RequestID;
    };

    class CPUUtilizationUpdate : public IEvent
    {
        EventType GetEventType() { return EventType::CPUUtilizationUpdate; };
    };

    class VMDuplicationRemoval : public IEvent
    {
        EventType GetEventType() { return EventType::VMDuplicationRemoval; };
    };

}

#endif // EVENT_TYPES_H