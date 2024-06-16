#ifndef DIALOGPTR_H
#define DIALOGPTR_H

#include <adwaita.h>
#include "dialogbase.h"

namespace Nickvision::TubeConverter::GNOME::Helpers
{
    template<typename T>
    concept DerivedDialogBase = std::is_base_of_v<DialogBase, T>;

    /**
     * @brief A pointer for a custom AdwDialog.
     * @brief Keeps the pointer alive until the dialog has closed.
     */
    template<DerivedDialogBase T>
    class DialogPtr
    {
    public:
        /**
         * @brief Constructs a DialogPtr.
         * @param args The arguments to pass to T's constructor.
         */
        template<typename... Args>
        DialogPtr(Args... args)
            : m_ptr{ new T(args...) }
        {
            g_signal_connect(m_ptr->get(), "closed", GCallback(+[](AdwDialog*, gpointer data){ delete reinterpret_cast<T*>(data); }), m_ptr);
        }
        /**
         * @brief Constructs a DialogPtr.
         * @brief The DialogPtr assumes ownership of ptr.
         * @param ptr T*
         */
        DialogPtr(T* ptr)
            : m_ptr{ ptr }
        {
            g_signal_connect(m_ptr->get(), "closed", GCallback(+[](AdwDialog*, gpointer data){ delete reinterpret_cast<T*>(data); }), m_ptr);
        }
        /**
         * @brief Returns the underlying pointer.
         * @return T*
         */
        T* operator->()
        {
            return m_ptr;
        }

    private:
        T* m_ptr;
    };
}

#endif //DIALOGPTR_H