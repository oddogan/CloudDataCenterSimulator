#include "RequestManager.h"

RequestManager::RequestManager(size_t BundleSize_)
{
    m_MaxBundleSize = BundleSize_;
    m_RequestBundle.reserve(2 * m_MaxBundleSize);
}

RequestManager::~RequestManager()
{
}

void RequestManager::AddRequestToBundle(EventTypes::Request* request)
{
    m_RequestBundle.push_back(*request);
}