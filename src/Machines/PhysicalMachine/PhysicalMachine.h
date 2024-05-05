#ifndef CDC_PHYSICALMACHINE_H
#define CDC_PHYSICALMACHINE_H

#include <iostream>
#include <iomanip>
#include <vector>
#include "../VirtualMachine/VirtualMachine.h"

class PhysicalMachine 
{
	public:
		PhysicalMachine(const unsigned int uiCoreCount_, const double dRAM_, const double dDisk_, const double dBandwidth_)
		{
			m_uiMachineID = m_uiGlobalMachineCount++;
			m_uiCoreAvailable = m_uiCoreCapacity = uiCoreCount_;
			m_dRAMAvailable = m_dRAMCapacity = dRAM_;
			m_dDiskAvailable = m_dDiskCapacity = dDisk_;
			m_dBandwidthAvailable = m_dBandwidthCapacity = dBandwidth_;
			m_bIsPoweredOn = false;
			m_dPowerUsage = 0.0;
		}

		/* Getters */
		unsigned int GetMachineID() const { return m_uiMachineID; }
		unsigned int GetAvailableCoreCount() const { return m_uiCoreAvailable; }
		double GetAvailableRAM() const { return m_dRAMAvailable; }
		double GetAvailableDisk() const { return m_dDiskAvailable; }
		double GetAvailableBandwidth() const { return m_dBandwidthAvailable; }

		double GetUtilizationCPU() const { return (static_cast<double>(m_uiCoreAvailable) / m_uiCoreCapacity); }
		double GetUtilizationRAM() const { return (m_dRAMAvailable / m_dRAMCapacity); }
		double GetUtilizationDisk() const { return (m_dDiskAvailable / m_dDiskCapacity); }
		double GetUtilizationBW() const { return (m_dBandwidthAvailable / m_dBandwidthCapacity); }

		bool IsPoweredOn() const { return m_bIsPoweredOn; }

		/* Setters */
		void TurnOn() 
		{ 
			m_bIsPoweredOn = true; 
			std::clog << "Physical machine " << m_uiMachineID << " is turned ON!" << std::endl;
		}
		void TurnOff() 
		{ 
			m_bIsPoweredOn = false; 
			std::clog << "Physical machine " << m_uiMachineID << " is turned OFF!" << std::endl;
		}

		void AddVM(unsigned int VMID_, unsigned int CoresReqs_, double RAMReq_, double DiskReq_, double BandwidthReq_)
		{
			if (IsPoweredOn() == false) TurnOn();

			m_uiCoreAvailable -= CoresReqs_;
			m_dRAMAvailable -= RAMReq_;
			m_dDiskAvailable -= DiskReq_;
			m_dBandwidthAvailable -= BandwidthReq_;

			m_VirtualMachines.emplace_back(VMID_, CoresReqs_, RAMReq_, DiskReq_, BandwidthReq_);
		}

		void RemoveVM(unsigned int VMID_)
		{
			for (auto i = 0; i < m_VirtualMachines.size(); ++i)
			{
				if (m_VirtualMachines[i].m_ID == VMID_)
				{
					m_uiCoreAvailable += m_VirtualMachines[i].m_Cores;
					m_dRAMAvailable += m_VirtualMachines[i].m_RAM;
					m_dDiskAvailable += m_VirtualMachines[i].m_Disk;
					m_dBandwidthAvailable += m_VirtualMachines[i].m_Bandwidth;

					m_VirtualMachines.erase(m_VirtualMachines.begin() + i);
					break;
				}
			}

			if ((m_uiCoreAvailable == m_uiCoreCapacity) &&
				(m_dRAMAvailable == m_dRAMCapacity) &&
				(m_dDiskAvailable == m_dDiskCapacity) &&
				(m_dBandwidthAvailable == m_dBandwidthCapacity))
			{
				TurnOff();
			}

		}

		/* Display */
		void DisplayInfo() const
		{
    		std::cout << std::setprecision(2) << std::fixed;
			std::cout << "Physical Machine ID: " << m_uiMachineID << " -> "
			<< "CPU: " << m_uiCoreAvailable << "/" << m_uiCoreCapacity << "cores"
			<< ", RAM: " << m_dRAMAvailable << "/" << m_dRAMCapacity << " GB"
			<< ", Disk: " << m_dDiskAvailable << "/" << m_dDiskCapacity << " GB"
			<< ", Bandwidth: " << m_dBandwidthAvailable  << "/" << m_dBandwidthCapacity << " MBps"
			<< ", Machine is powered " << (m_bIsPoweredOn ? "ON" : "OFF") << std::endl;
		}

	private:
		/* Identifier */
		unsigned int m_uiMachineID;

		/* Attributes of a physical machine */
		unsigned int m_uiCoreCapacity;
		double m_dRAMCapacity;
		double m_dDiskCapacity;
		double m_dBandwidthCapacity;

		/* Status */
		unsigned int m_uiCoreAvailable;
		double m_dRAMAvailable;
		double m_dDiskAvailable;
		double m_dBandwidthAvailable;
		bool m_bIsPoweredOn;
		double m_dPowerUsage;

		/* Virtual machines inside */
		std::vector<VirtualMachine> m_VirtualMachines;

		/* Static */
		static unsigned int m_uiGlobalMachineCount;
};

#endif  // PHYSICALMACHINE_H
