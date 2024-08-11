#ifndef CONTROLBASE_H
#define CONTROLBASE_H

#include <string>
#include <adwaita.h>
#include "builder.h"

namespace Nickvision::TubeConverter::GNOME::Helpers
{
    /**
     * @brief A base class for custom gtk controls with blueprints.
     */
    template<typename T>
    class ControlBase
    {
    public:
        /**
         * @brief Constructs a ControlBase.
         * @param parent GtkWindow*
         * @param fileName The file name for the blueprint file of the control
         * @param rootName The name of the control component in the blueprint file
         */
        ControlBase(GtkWindow* parent, const std::string& fileName, const std::string& rootName = "root")
            : m_builder{ fileName },
            m_parent{ parent },
            m_control{ m_builder.get<T>(rootName) } 
        {

        }
        /**
         * @brief Gets the underlying control pointer.
         * @return T*
         */
        T* gobj()
        {
            return m_control;
        }
        
    protected:
        Builder m_builder;
        GtkWindow* m_parent;
        T* m_control;
    };
}

#endif //CONTROLBASE_H