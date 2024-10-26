#ifndef CDC_CLOUDDATACENTER_H
#define CDC_CLOUDDATACENTER_H

#include <vector>
#include "../Events/EventTypes.h"
#include "../EventDispatcher/EventDispatcher.h"
#include "../RequestManager/RequestManager.h"
#include "../GlobalTimer/GlobalTimer.h"
#include "../MachineRoom/MachineRoom.h"
#include "../AllocationStrategies/IAllocationStrategy.h"
#include "../StatisticsManager/StatisticsManager.h"

class CloudDataCenter
{
	public:
		CloudDataCenter();

		// Component setters/getters
		void SetEventDispatcher(EventDispatcher *eventDispatcher);
		void SetMachineRoom(MachineRoom *machineRoom);
		void SetAllocationStrategy(IAllocationStrategy *strategy);
		void SetRequestManager(RequestManager *requestManager);
		EventDispatcher* GetEventDispatcher();
		MachineRoom* GetMachineRoom();
		IAllocationStrategy* GetAllocationStrategy();
		RequestManager* GetRequestManager();

		// Start the operation
		bool StartOperation();

		// Get information about the data center
		void DisplayStatus() const;
		void WaitForFinish() { m_MainControllerThread.join(); }
		bool IsCurrentlyActive() const { return m_IsCurrentlyActive; }

	private:
    	EventDispatcher* m_EventDispatcher;
    	MachineRoom* m_MachineRoom;
    	RequestManager* m_RequestManager;
        IAllocationStrategy* m_AllocationStrategy;

		/* Main control loop */
		bool m_IsCurrentlyActive;
		void MainController();
		std::thread m_MainControllerThread;

		/* Assignment */
		void ProcessRequests();
};

#endif // CLOUDDATACENTER_H
