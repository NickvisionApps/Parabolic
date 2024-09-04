#ifndef BUILDER_H
#define BUILDER_H

#include <string>
#include <adwaita.h>

namespace Nickvision::TubeConverter::GNOME::Helpers
{
    /**
     * @brief A helper class wrapping GtkBuilder functionality.
     */
    class Builder
    {
    public:
        /**
         * @brief Constructs a Builder.
         * @brief Looks for the ui file with the path: "{CURRENT_DIRECTORY}/ui/{uiFileName}.ui"
         * @param uiFileName The name of the ui file
         * @throw std::invalid_argument Thrown if the file does not exist
         * @throw std::runtime_error Thrown if unable to create the GtkBuilder object
         */
        Builder(const std::string& uiFileName);
        /**
         * @brief Destructs a Builder.
         */
        ~Builder();
        /**
         * @brief Gets the GtkBuilder object.
         * @return The GtkBuilder object
         */
        GtkBuilder* gobj() const;
        /**
         * @brief Gets a widget from the GtkBuilder object.
         * @param name The name of the widget
         * @return The widget or nullptr if not found
         */
        template<typename T>
        T* get(const std::string& name) const
        {
            return reinterpret_cast<T*>(gtk_builder_get_object(m_builder, name.c_str()));
        }

    private:
        GtkBuilder* m_builder;
    };
}

#endif //BUILDER_H