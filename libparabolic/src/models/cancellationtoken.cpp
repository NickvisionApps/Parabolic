#include "models/cancellationtoken.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    CancellationToken::CancellationToken(const std::function<void()>& cancelFunction)
        : m_cancelled{ false },
        m_cancelFunction{ cancelFunction }
    {

    }

    bool CancellationToken::isCancelled() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_cancelled;
    }

    const std::function<void()>& CancellationToken::getCancelFunction() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_cancelFunction;
    }

    void CancellationToken::setCancelFunction(const std::function<void()>& cancelFunction)
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        m_cancelFunction = cancelFunction;
    }

    void CancellationToken::cancel()
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        if(m_cancelled)
        {
            return;
        }
        m_cancelled = true;
        if(m_cancelFunction)
        {
            lock.unlock();
            m_cancelFunction();
        }
    }

    CancellationToken::operator bool() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_cancelled;
    }
}