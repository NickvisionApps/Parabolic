#include "views/keyringpage.h"
#include <libnick/localization/gettext.h>

using namespace Nickvision::Keyring;
using namespace Nickvision::TubeConverter::Shared::Controllers;

namespace Nickvision::TubeConverter::GNOME::Views
{
    KeyringPage::KeyringPage(const std::shared_ptr<KeyringDialogController>& controller, GtkWindow* parent)
        : ControlBase{ parent, "keyring_page" },
        m_controller{ controller }
    {
        //Signals
        g_signal_connect(m_builder.get<GObject>("addCredentialNoneButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<KeyringPage*>(data)->addNewCredential(); }), this);
        g_signal_connect(m_builder.get<GObject>("addCredentialButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<KeyringPage*>(data)->addNewCredential(); }), this);
        //Load
        reloadCredentials();
    }

    void KeyringPage::reloadCredentials()
    {
        for(const Credential& credential : m_controller->getCredentials())
        {

        }
        adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("viewStack"), m_controller->getCredentials().empty() ? "no-credentials" : "credentials");
    }
    
    void KeyringPage::addNewCredential()
    {

    }
}