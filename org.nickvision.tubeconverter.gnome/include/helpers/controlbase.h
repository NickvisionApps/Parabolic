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
            : m_builder{ BuilderHelpers::fromBlueprint(fileName) },
            m_parent{ parent },
            m_control{ reinterpret_cast<T*>(gtk_builder_get_object(m_builder, rootName.c_str())) }
        {

        }
        /**
         * @brief Destructs a ControlBase.
         */
        virtual ~ControlBase()
        {
            g_object_unref(m_builder);
        }
        /**
         * @brief Gets the underlying control pointer.
         * @return T*
         */
        T* get()
        {
            return m_control;
        }
        
    protected:
        GtkBuilder* m_builder;
        GtkWindow* m_parent;
        T* m_control;
    };
}

#endif //CONTROLBASE_H