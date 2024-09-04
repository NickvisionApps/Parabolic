#include "helpers/dialogbase.h"

using namespace Nickvision::Events;

namespace Nickvision::TubeConverter::GNOME::Helpers
{
    DialogBase::DialogBase(GtkWindow* parent, const std::string& fileName, const std::string& rootName)
        : m_builder{ fileName },
        m_parent{ parent },
        m_dialog{ m_builder.get<AdwDialog>(rootName) }
    {

    }

    AdwDialog* DialogBase::gobj()
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
