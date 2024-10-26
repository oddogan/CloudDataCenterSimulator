#include "PhysicalMachine.h"

unsigned int PhysicalMachine::m_uiGlobalMachineCount = 0;

PhysicalMachine::PhysicalMachine(PhysicalMachineResources& resourceSpecs, PhysicalMachinePowerSpecs& powerSpecs)
{
	this->resourceSpecs = resourceSpecs;
	this->powerSpecs = powerSpecs;
	this->availableResources = this->resourceSpecs;

	m_uiMachineID = m_uiGlobalMachineCount++;
	m_IsPoweredOn = false;
	m_dPowerUsage = 0.0;
	m_UsageCPU    = 1.0;
	m_MigrationStatus = false;

	m_VirtualMachines.reserve(10e3);
}

void PhysicalMachine::TurnOn()
{
	m_IsPoweredOn = true;
	m_dPowerUsage += powerSpecs.Base;
	std::cout << "Physical machine " << m_uiMachineID << " is turned ON!" << std::endl;
}

void PhysicalMachine::TurnOff()
{
	m_IsPoweredOn = false;
	m_dPowerUsage = 0;
	std::cout << "Physical machine " << m_uiMachineID << " is turned OFF!" << std::endl;
}

void PhysicalMachine::AddVM(EventTypes::Request& request)
{
	if (IsPoweredOn() == false) TurnOn();

	auto resources = request.m_RequestedResources;
	resources.CoreCount *= request.m_CurrentUtilization / 100.f;

	availableResources -= resources;
	m_dPowerUsage += powerSpecs.CPU * resources.CoreCount;

	if (GetUtilizationCPU() > 1) throw std::runtime_error("Exceeded CPU utilization!");
	else if (GetUtilizationCPU() < 0) throw std::runtime_error("Negative CPU utilization!");

	m_VirtualMachines.emplace_back(request.m_RequestID, std::move(request), resources);
}

void PhysicalMachine::RemoveVM(VIRTUAL_MACHINE_ID vmID)
{
	auto VM = std::find_if(m_VirtualMachines.begin(), m_VirtualMachines.end(), [vmID](const VirtualMachine& vm){ return vm.m_ID == vmID; });
	if (VM != m_VirtualMachines.end())
	{
		availableResources += VM->m_CurrentlyUsedResources;
		m_dPowerUsage -= powerSpecs.CPU * VM->m_CurrentlyUsedResources.CoreCount;
		m_VirtualMachines.erase(VM);

		if (GetUtilizationCPU() > 1) throw std::runtime_error("Exceeded CPU utilization!");
		else if (GetUtilizationCPU() < 0) throw std::runtime_error("Negative CPU utilization!");
	}
	else
	{
		std::cerr << "Failed to find VM " << vmID << " in PM " << m_uiMachineID << std::endl;
		throw std::runtime_error("VM not found in the PM");
	}

	if (m_VirtualMachines.size() == 0)
	{
		TurnOff();
	}
}

void PhysicalMachine::UpdateVMUtilization(VIRTUAL_MACHINE_ID vmID, double utilizationValue)
{
	auto VM = std::find_if(m_VirtualMachines.begin(), m_VirtualMachines.end(), [vmID](const VirtualMachine& vm){ return vm.m_ID == vmID; });
	if (VM != m_VirtualMachines.end())
	{
		availableResources += VM->m_CurrentlyUsedResources;
		m_dPowerUsage -= powerSpecs.CPU * VM->m_CurrentlyUsedResources.CoreCount;

		VM->m_Request.m_CurrentUtilization = utilizationValue;
		VM->m_CurrentlyUsedResources.CoreCount = VM->m_Request.m_RequestedResources.CoreCount * utilizationValue / 100.f;

		availableResources -= VM->m_CurrentlyUsedResources;
		m_dPowerUsage += powerSpecs.CPU * VM->m_CurrentlyUsedResources.CoreCount;
		
		if (GetUtilizationCPU() < 0) throw std::runtime_error("Negative CPU utilization!");
	}
	else
	{
		std::cerr << "Failed to find VM " << vmID << " in PM " << m_uiMachineID << std::endl;
		throw std::runtime_error("VM not found in the PM");
	}
}

VirtualMachine* PhysicalMachine::GetVM(VIRTUAL_MACHINE_ID vmID)
{
	auto VM = std::find_if(m_VirtualMachines.begin(), m_VirtualMachines.end(), [vmID](const VirtualMachine& vm){ return vm.m_ID == vmID; });
	if (VM != m_VirtualMachines.end())
	{
		return &(*VM);
	}
	else
	{
		std::cerr << "Failed to find VM " << vmID << " in PM " << m_uiMachineID << std::endl;
		throw std::runtime_error("VM not found in the PM");
	}
}

void PhysicalMachine::DisplayInfo() const
{
    std::cout << std::setprecision(2) << std::fixed;
	std::cout << "Physical Machine ID: " << m_uiMachineID << " -> "
	<< "CPU: " << availableResources.CoreCount << "/" << resourceSpecs.CoreCount << " cores"
	<< ", RAM: " << availableResources.RAM << "/" << resourceSpecs.RAM << " GB"
	<< ", Disk: " << availableResources.Disk << "/" << resourceSpecs.Disk << " GB"
	<< ", Bandwidth: " << availableResources.Bandwidth  << "/" << resourceSpecs.Bandwidth << " MBps"
	<< ", Machine is powered " << (m_IsPoweredOn ? "ON" : "OFF") << std::endl;
}
