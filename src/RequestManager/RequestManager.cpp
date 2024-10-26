#include "RequestManager.h"

RequestManager::RequestManager()
{
    m_MaxBundleSize = 1;
}

RequestManager::~RequestManager()
{
}

void RequestManager::SetBundleSize(size_t bundleSize)
{
    m_MaxBundleSize = bundleSize;
}

void RequestManager::AddRequestToBundle(EventTypes::Request* request)
{
    m_RequestBundle.push_back(*request);
}
