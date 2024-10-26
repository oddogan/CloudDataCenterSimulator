#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include "../Events/EventTypes.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include "../Machines/PhysicalMachine/PhysicalMachine.h"

class RequestManager
{
    public:
        RequestManager();
        virtual ~RequestManager();

        /* Bundle operations */
        void SetBundleSize(size_t bundleSize);
        void AddRequestToBundle(EventTypes::Request* request);
        bool BundleFilled() { return m_RequestBundle.size() == m_MaxBundleSize; }
        void ClearBundle() { m_RequestBundle.clear(); }
        std::vector<EventTypes::Request>& GetRequestBundle() { return m_RequestBundle; }

        /* VM vs PhysicalMachines mapping operations */
        PhysicalMachine* GetVMOwnerPhysicalMachine(VIRTUAL_MACHINE_ID vmID)
        {
            if (m_VMvsPM_Map.find(vmID) != m_VMvsPM_Map.end())
            {
                return m_VMvsPM_Map[vmID];
            }
            else
            {
                throw std::runtime_error("Could not find VM owner PM!");
                return nullptr;
            }
        }

        void SetVMOwnerPhysicalMachine(VIRTUAL_MACHINE_ID vmID, PhysicalMachine& machine_)
        {
            m_VMvsPM_Map[vmID] = &machine_;
        }

    private:
        std::vector<EventTypes::Request> m_RequestBundle;
        std::unordered_map<VIRTUAL_MACHINE_ID, PhysicalMachine*> m_VMvsPM_Map;
        size_t m_MaxBundleSize;
};

#endif // REQUESTMANAGER_H
