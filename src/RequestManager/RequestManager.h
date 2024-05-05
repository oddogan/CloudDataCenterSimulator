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
        RequestManager(size_t BundleSize_ = 3);
        virtual ~RequestManager();

        /* Bundle operations */
        void AddRequestToBundle(EventTypes::Request* request);
        bool BundleFilled() { return m_RequestBundle.size() == m_MaxBundleSize; }
        void ClearBundle() { m_RequestBundle.clear(); }
        const std::vector<EventTypes::Request>& GetRequestBundle() { return m_RequestBundle; }

        /* VM vs PhysicalMachines mapping operations */
        PhysicalMachine* GetVMOwnerPhysicalMachine(unsigned int uiVMID)
        {
            if (m_VMvsPM_Map.find(uiVMID) != m_VMvsPM_Map.end())
            {
                return m_VMvsPM_Map[uiVMID];
            }
            else
            {
                return nullptr;
            }
        }

        void SetVMOwnerPhysicalMachine(unsigned int VMID_, PhysicalMachine& machine_)
        {
            m_VMvsPM_Map[VMID_] = &machine_;
        }

    private:
        std::vector<EventTypes::Request> m_RequestBundle;
        std::unordered_map<unsigned int, PhysicalMachine*> m_VMvsPM_Map;
        size_t m_MaxBundleSize;
};

#endif // REQUESTMANAGER_H