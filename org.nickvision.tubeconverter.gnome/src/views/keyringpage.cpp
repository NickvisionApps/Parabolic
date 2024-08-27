#include "views/keyringpage.h"
#include <libnick/localization/gettext.h>

using namespace Nickvision::TubeConverter::Shared::Controllers;

namespace Nickvision::TubeConverter::GNOME::Views
{
    KeyringPage::KeyringPage(const std::shared_ptr<KeyringDialogController>& controller, GtkWindow* parent)
        : ControlBase{ parent, "keyring_page" },
        m_controller{ controller }
    {

    }
}