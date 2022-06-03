#include "shortcutsdialog.h"

using namespace NickvisionTubeConverter::UI;
using namespace NickvisionTubeConverter::UI::Views;

ShortcutsDialog::ShortcutsDialog(GtkWidget* parent) : Widget{"/org/nickvision/tubeconverter/ui/views/shortcutsdialog.xml", "gtk_shortcutsDialog"}
{
    //==Dialog==//
    gtk_window_set_transient_for(GTK_WINDOW(m_gobj), GTK_WINDOW(parent));
}

ShortcutsDialog::~ShortcutsDialog()
{
    gtk_window_destroy(GTK_WINDOW(m_gobj));
}
