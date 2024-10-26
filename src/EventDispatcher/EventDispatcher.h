#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include "../GlobalTimer/GlobalTimer.h"
#include "../Events/EventTypes.h"
#include <queue>
#include <mutex>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <fstream>

class EventDispatcher
{
	public:
		EventDispatcher();
		virtual ~EventDispatcher();

		void EventReader(const std::vector<std::string>& traceFilenames);

		IEvent* GetEvent();
		void AddEvent(IEvent* event_);

	private:
		std::mutex m_mtxProtectQueue;
		std::condition_variable m_condVariable;
		std::priority_queue<IEvent*, std::vector<IEvent*>, CompareEvents> m_EventQueue;

		[[nodiscard]] std::vector<std::string> ParseStringViaDelimiter(std::string line_, const char* delimiter_);
		void RandomEventGenerator();
};

#endif  // EVENTDISPATCHER_H
