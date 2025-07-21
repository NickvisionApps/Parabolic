#ifndef CANCELLATIONTOKEN_H
#define CANCELLATIONTOKEN_H

#include <functional>
#include <mutex>

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A token that can be used to cancel an operation.
     */
    class CancellationToken
    {
    public:
        /**
         * @brief Constructs a CancellationToken.
         * @param cancelFunction A callback function to call when the token is cancelled
         */
        CancellationToken(const std::function<void()>& cancelFunction = {});
        /**
         * @brief Gets whether or not the token is cancelled.
         * @return True if token is cancelled, else false
         */
        bool isCancelled() const;
        /**
         * @brief Gets the cancel function to be called when the token is cancelled.
         * @return The cancel function
         */
        const std::function<void()>& getCancelFunction() const;
        /**
         * @brief Sets the cancel function to be called when the token is cancelled.
         * @param cancelFunction The cancel function
         */
        void setCancelFunction(const std::function<void()>& cancelFunction);
        /**
         * @brief Cancels the token.
         */
        void cancel();
        /**
         * @brief Converts the token to a boolean.
         * @return True if token is cancelled, else false
         */
        operator bool() const;

    private:
        mutable std::mutex m_mutex;
        bool m_cancelled;
        std::function<void()> m_cancelFunction;
    };
}

#endif //CANCELLATIONTOKEN_H