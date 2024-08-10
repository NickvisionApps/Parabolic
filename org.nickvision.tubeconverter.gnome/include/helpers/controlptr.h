#ifndef CONTROLPTR_H
#define CONTROLPTR_H

#include <adwaita.h>
#include "controlbase.h"

namespace Nickvision::TubeConverter::GNOME::Helpers
{
    template<typename T>
    concept DerivedControlBase = std::is_base_of_v<ControlBase, T>;

    /**
     * @brief A pointer for a custom gtk control.
     * @brief Keeps the pointer alive until the control has been destroyed.
     */
    template<DerivedControlBase T>
    class ControlPtr
    {
    public:
        /**
         * @brief Constructs a ControlPtr.
         * @param args the arguments to pass to T's constructor.
         */
        template<typename... Args>
        ControlPtr(Args... args)
            : m_ptr{ new T(args...) }
        {
            g_signal_connect(m_ptr->get(), "destroy", GCallback(+[](GtkWidget*, gpointer data){ delete reinterpret_cast<T*>(data); }), m_ptr);
        }
        /**
         * @brief Constructs a ControlPtr.
         * @brief The ControlPtr assumes ownership of ptr.
         * @param ptr T*
         */
        ControlPtr(T* ptr)
            : m_ptr{ ptr }
        {
            g_signal_connect(m_ptr->get(), "destroy", GCallback(+[](GtkWidget*, gpointer data){ delete reinterpret_cast<T*>(data); }), m_ptr);
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

#endif //CONTROLPTR_H