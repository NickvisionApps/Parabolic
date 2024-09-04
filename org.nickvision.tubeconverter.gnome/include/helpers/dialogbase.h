#ifndef DIALOGBASE_H
#define DIALOGBASE_H

#include <string>
#include <adwaita.h>
#include <libnick/events/event.h>
#include <libnick/events/eventargs.h>
#include "builder.h"

namespace Nickvision::TubeConverter::GNOME::Helpers
{
    /**
     * @brief A base class for custom AdwDialogs with blueprints.
     */
    class DialogBase
    {
    public:
        /**
         * @brief Constructs a DialogBase.
         * @param parent GtkWindow*
         * @param fileName The file name for the blueprint file of the dialog
         * @param rootName The name of the AdwDialog component in the blueprint file
         */
        DialogBase(GtkWindow* parent, const std::string& fileName, const std::string& rootName = "root");
        /**
         * @brief Gets the underlying AdwDialog pointer.
         * @return AdwDialog*
         */
        AdwDialog* gobj();
        /**
         * @brief Gets the event for when the dialog is closed.
         * @return The closed event
         */
        Nickvision::Events::Event<Nickvision::Events::EventArgs>& closed();
        /**
         * @brief Presents the AdwDialog.
         */
        void present() const;

    protected:
        Builder m_builder;
        GtkWindow* m_parent;
        AdwDialog* m_dialog;
        Nickvision::Events::Event<Nickvision::Events::EventArgs> m_closed;
    };
}

#endif //DIALOGBASE_H
