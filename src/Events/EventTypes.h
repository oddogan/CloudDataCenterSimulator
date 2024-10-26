#ifndef EVENT_TYPES_H
#define EVENT_TYPES_H

#include <vector>
#include "Event.h"
#include "../Machines/Resources.h"

namespace EventTypes
{
    class Request : public IEvent
    {
        public:
            Request() 
            {
                m_utilizationUpdateValues.reserve(1e3);
            }
            virtual ~Request() {};
            EventType GetEventType() { return EventType::Request; };
            double m_RequestDuration;
            unsigned int m_RequestID;
            PhysicalMachineResources m_RequestedResources;
            double m_CurrentUtilization;
            size_t m_UtilizationUpdateCounter;
            std::vector<double> m_utilizationUpdateValues;
    };

    class MigrationRequest : public IEvent
    {
        public:
            MigrationRequest() {}
            virtual ~MigrationRequest() {};
            EventType GetEventType() { return EventType::MigrationRequest; };
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
        public:
            EventType GetEventType() { return EventType::CPUUtilizationUpdate; };
            unsigned int m_RequestID;
            double m_UtilizationValue;
    };

    class VMDuplicationRemoval : public IEvent
    {
        EventType GetEventType() { return EventType::VMDuplicationRemoval; };
    };

}

#endif // EVENT_TYPES_H