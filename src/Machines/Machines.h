#ifndef SMALLMACHINE_H
#define SMALLMACHINE_H

namespace MachineTypes
{
    namespace SmallMachine
    {
        constexpr int CPU_Min = 40;
        constexpr int CPU_Max = 56;
        constexpr double RAM_Min = 1024;
        constexpr double RAM_Max = 2048;
        constexpr double Disk_Min = 24'000;
        constexpr double Disk_Max = 46'000;
        constexpr double BW_Min = 40'000;
        constexpr double BW_Max = 60'000;
    }

    namespace BigMachine
    {
        constexpr int CPU_Min = 40;
        constexpr int CPU_Max = 40;
        constexpr double RAM_Min = 1024;
        constexpr double RAM_Max = 1024;
        constexpr double Disk_Min = 24'576;
        constexpr double Disk_Max = 24'576;
        constexpr double BW_Min = 40'000;
        constexpr double BW_Max = 40'000;
    }
}

#endif // SMALLMACHINE_H