#ifndef RESOURCES_H
#define RESOURCES_H

struct PhysicalMachineResources
{
    double CoreCount;
	unsigned int FPGA;
	double RAM;
	double Disk;
	double Bandwidth;

	bool operator== (const PhysicalMachineResources& rhs) const 
	{
    	return (this->CoreCount == rhs.CoreCount) && (this->RAM == rhs.RAM) && (this->Disk == rhs.Disk) && (this->Bandwidth == rhs.Bandwidth) && (this->FPGA < rhs.FPGA);
	}

	bool operator< (const PhysicalMachineResources& rhs) const 
	{
    	return (this->CoreCount < rhs.CoreCount) || (this->RAM < rhs.RAM) || (this->Disk < rhs.Disk) || (this->Bandwidth < rhs.Bandwidth) || (this->FPGA < rhs.FPGA);
	}

	PhysicalMachineResources& operator-= (const PhysicalMachineResources& rhs) 
	{
    	this->CoreCount -= rhs.CoreCount;
		this->RAM -= rhs.RAM;
		this->Disk -= rhs.Disk;
		this->Bandwidth -= rhs.Bandwidth;
		this->FPGA -= rhs.FPGA;
		return *this;
	}

	PhysicalMachineResources& operator+= (const PhysicalMachineResources& rhs) 
	{
    	this->CoreCount += rhs.CoreCount;
		this->RAM += rhs.RAM;
		this->Disk += rhs.Disk;
		this->Bandwidth += rhs.Bandwidth;
		this->FPGA -= rhs.FPGA;
		return *this;
	}
};

struct PhysicalMachinePowerSpecs
{
    double CPU;
	double Base;
};

#endif