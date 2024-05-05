#ifndef EVENT_H
#define EVENT_H

enum class EventType
{
    SimulationEnd = -1,
    IncomingRequest,
    Exit,
    CPUUtilizationUpdate,
    VMDuplicationRemoval
};

class IEvent
{
    public:
        IEvent() {};
        IEvent(double EventTime_) :
            m_EventTime(EventTime_) {}
        virtual ~IEvent() {};

        virtual EventType GetEventType() = 0;

        double m_EventTime;
};

class CompareEvents
{
    public:
        bool operator()(IEvent* olderEvent_, IEvent* newerEvent_)
        {
            if (olderEvent_->m_EventTime <= newerEvent_->m_EventTime)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
};

#endif  // EVENT_H
