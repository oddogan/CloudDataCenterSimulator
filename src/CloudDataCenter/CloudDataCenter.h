#ifndef CDC_CLOUDDATACENTER_H
#define CDC_CLOUDDATACENTER_H

#include <vector>
#include "../Machines/Machines.h"
#include "../Machines/PhysicalMachine/PhysicalMachine.h"
#include "../Events/EventTypes.h"
#include "../EventDispatcher/EventDispatcher.h"
#include "../RequestManager/RequestManager.h"
#include "../Utilities/util.h"
#include "../GlobalTimer/GlobalTimer.h"

class CloudDataCenter
{
	public:
		CloudDataCenter(EventDispatcher *EventDispatcher_);

		EventDispatcher *m_EventDispatcher;
		RequestManager m_RequestManager;

		/* Add a new physical machine to the data center */
		void AddPhysicalMachine(const PhysicalMachine &machine_);

		/* Remove a physical machine from the data center */
		void RemovePhysicalMachine(const unsigned int uiMachineID_);

		/* Get a physical machine's handler via machine ID */
		PhysicalMachine &GetPhysicalMachine(const unsigned int uiMachineID_);

		/* Get the vector of physical machines */
		const std::vector<PhysicalMachine> &GetPhysicalMachines() const;

		/* Get the number of physical machines */
		int GetPhysicalMachineCount() const;

		/* Display info about the data center */
		void DisplayStatus() const;

		void WaitForFinish() { m_MainControllerThread.join(); }
		bool IsCurrentlyActive() const { return m_IsCurrentlyActive; }

	private:
		/* Main control loop */
		bool m_IsCurrentlyActive;
		void MainController();
		std::thread m_MainControllerThread;

		/* Assignment */
		void ProcessRequests();

		/* Physical machines */
		std::vector<PhysicalMachine> m_PhysicalMachines;

		/* Construct the machines */
		void ConstructMachines(size_t iSmallMachineCount_, size_t iBigMachineCount_);
};

#endif // CLOUDDATACENTER_H
