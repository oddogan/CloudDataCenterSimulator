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
		EventDispatcher(const char* cpTraceFilename_, size_t szNumFiles_);
		virtual ~EventDispatcher();

		void Start();
		void WaitForFinish() { m_DispatcherThread.join(); };

		IEvent* GetEvent();
		void AddEvent(IEvent* event_);

	private:
		std::thread m_DispatcherThread;
		std::mutex m_mtxProtectQueue;
		std::condition_variable m_condVariable;
		std::priority_queue<IEvent*, std::vector<IEvent*>, CompareEvents> m_EventQueue;

		std::string m_TraceDatabasePrefix;
		int m_TraceNumFiles;

		void EventReader();
		[[nodiscard]] std::vector<std::string> ParseStringViaDelimiter(std::string line_, const char* delimiter_);
		void RandomEventGenerator();
};

#endif  // EVENTDISPATCHER_H
