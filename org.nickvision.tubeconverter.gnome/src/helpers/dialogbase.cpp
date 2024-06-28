#include "helpers/dialogbase.h"
#include "helpers/builder.h"

using namespace Nickvision::Events;

namespace Nickvision::TubeConverter::GNOME::Helpers
{
    DialogBase::DialogBase(GtkWindow* parent, const std::string& fileName, const std::string& rootName)
        : m_builder{ BuilderHelpers::fromBlueprint(fileName) },
        m_parent{ parent },
        m_dialog{ ADW_DIALOG(gtk_builder_get_object(m_builder, rootName.c_str())) }
    {

    }

    DialogBase::~DialogBase()
    {
        m_closed.invoke({});
        adw_dialog_force_close(m_dialog);
        g_object_unref(m_builder);
    }

    AdwDialog* DialogBase::get()
    {
        return m_dialog;
    }

    Event<EventArgs>& DialogBase::closed()
    {
        return m_closed;
    }

    void DialogBase::present() const
    {
        adw_dialog_present(m_dialog, GTK_WIDGET(m_parent));
    }
}
