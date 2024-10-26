#ifndef CDC_PHYSICALMACHINE_H
#define CDC_PHYSICALMACHINE_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include "../VirtualMachine/VirtualMachine.h"
#include "../Resources.h"

using PHYSICAL_MACHINE_ID = size_t;

class PhysicalMachine
{
	public:
		PhysicalMachine(PhysicalMachineResources& resourceSpecs, PhysicalMachinePowerSpecs& powerSpecs);

		/* Getters */
		PHYSICAL_MACHINE_ID GetMachineID() const { return m_uiMachineID; }
		const PhysicalMachineResources& GetResourceSpecs() const { return resourceSpecs; }
		const PhysicalMachinePowerSpecs& GetPowerSpecs() const { return powerSpecs; }
		const PhysicalMachineResources& GetAvailableResources() const { return availableResources; }

		double GetUtilizationCPU() const { return (1.0 - availableResources.CoreCount / resourceSpecs.CoreCount); }
		double GetUtilizationRAM() const { return (1.0 - availableResources.RAM / resourceSpecs.RAM); }
		double GetUtilizationDisk() const { return (1.0 - availableResources.Disk / resourceSpecs.Disk); }
		double GetUtilizationBW() const { return (1.0 - availableResources.Bandwidth / resourceSpecs.Bandwidth); }

		bool IsPoweredOn() const { return m_IsPoweredOn; }
		bool GetMigrationStatus() { return m_MigrationStatus; }
		void SetMigrationStatus(bool status) { m_MigrationStatus = status; }

		/* Setters */
		void TurnOn();
		void TurnOff();

		void AddVM(EventTypes::Request& request);
		void RemoveVM(VIRTUAL_MACHINE_ID vmID);
		void UpdateVMUtilization(VIRTUAL_MACHINE_ID vmID, double utilizationValue);
		VirtualMachine* GetVM(VIRTUAL_MACHINE_ID vmID);

		/* Display */
		void DisplayInfo() const;

	private:
		/* Identifier */
		PHYSICAL_MACHINE_ID m_uiMachineID;

		/* Attributes of a physical machine */
		PhysicalMachineResources resourceSpecs;
		PhysicalMachinePowerSpecs powerSpecs;

		/* Status */
		PhysicalMachineResources availableResources;
		bool m_IsPoweredOn;
		double m_dPowerUsage;
		double m_UsageCPU;
		bool m_MigrationStatus;

		/* Virtual machines inside */
		std::vector<VirtualMachine> m_VirtualMachines;

		/* Static */
		static unsigned int m_uiGlobalMachineCount;
};

#endif  // PHYSICALMACHINE_H
