#include "views/adddownloaddialog.h"
#include <libnick/localization/gettext.h>

using namespace Nickvision::Events;
using namespace Nickvision::TubeConverter::Shared::Controllers;

namespace Nickvision::TubeConverter::GNOME::Views
{
    AddDownloadDialog::AddDownloadDialog(const std::shared_ptr<AddDownloadDialogController>& controller, GtkWindow* parent)
        : DialogBase{ parent, "add_download_dialog" },
        m_controller{ controller }
    {
        //Signals
        m_closed += [&](const EventArgs&) { onClosed(); };
    }

    void AddDownloadDialog::onClosed()
    {
        
    }
}