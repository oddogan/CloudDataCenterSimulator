#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

class VirtualMachine
{
    public:
        VirtualMachine(unsigned int VMID_, unsigned int CoresReqs_, double RAMReq_, double DiskReq_, double BandwidthReq_) 
        {
            m_ID = VMID_;
            m_Cores = CoresReqs_;
            m_RAM = RAMReq_;
            m_Disk = DiskReq_;
            m_Bandwidth = BandwidthReq_;
        };
        
        unsigned int m_ID;
        unsigned int m_Cores;
        double m_RAM;
        double m_Disk;
        double m_Bandwidth;
};

#endif // VIRTUAL_MACHINE_H