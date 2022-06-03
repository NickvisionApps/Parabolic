#include "preferencesdialog.h"

using namespace NickvisionTubeConverter::Models;
using namespace NickvisionTubeConverter::UI;
using namespace NickvisionTubeConverter::UI::Views;

PreferencesDialog::PreferencesDialog(GtkWidget* parent, Configuration& configuration) : Widget{"/org/nickvision/tubeconverter/ui/views/preferencesdialog.xml", "adw_preferencesDialog"}, m_configuration{configuration}
{
    //==Dialog==//
    gtk_window_set_transient_for(GTK_WINDOW(m_gobj), GTK_WINDOW(parent));
    //==Signals==//
    g_signal_connect(gtk_builder_get_object(m_builder, "gtk_btnCancel"), "clicked", G_CALLBACK((void (*)(GtkButton*, gpointer*))[](GtkButton* button, gpointer* data) { reinterpret_cast<PreferencesDialog*>(data)->cancel(); }), this);
    g_signal_connect(gtk_builder_get_object(m_builder, "gtk_btnSave"), "clicked", G_CALLBACK((void (*)(GtkButton*, gpointer*))[](GtkButton* button, gpointer* data) { reinterpret_cast<PreferencesDialog*>(data)->save(); }), this);
    g_signal_connect(gtk_builder_get_object(m_builder, "adw_rowIsFirstTimeOpen"), "activated", G_CALLBACK((void (*)(AdwActionRow*, gpointer*))[](AdwActionRow* row, gpointer* data) { reinterpret_cast<PreferencesDialog*>(data)->onRowIsFirstTimeOpenActivate(); }), this);
    //==Load Config==//
    if(m_configuration.getTheme() == Theme::System)
    {
        adw_combo_row_set_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "adw_rowTheme")), 0);
    }
    else if(m_configuration.getTheme() == Theme::Light)
    {
        adw_combo_row_set_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "adw_rowTheme")), 1);
    }
    else if(m_configuration.getTheme() == Theme::Dark)
    {
        adw_combo_row_set_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "adw_rowTheme")), 2);
    }
    gtk_switch_set_active(GTK_SWITCH(gtk_builder_get_object(m_builder, "gtk_switchIsFirstTimeOpen")), m_configuration.getIsFirstTimeOpen());
}

PreferencesDialog::~PreferencesDialog()
{
    gtk_window_destroy(GTK_WINDOW(m_gobj));
}

void PreferencesDialog::cancel()
{
    gtk_widget_hide(m_gobj);
}

void PreferencesDialog::save()
{
    m_configuration.setTheme(static_cast<Theme>(adw_combo_row_get_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "adw_rowTheme")))));
    m_configuration.setIsFirstTimeOpen(gtk_switch_get_active(GTK_SWITCH(gtk_builder_get_object(m_builder, "gtk_switchIsFirstTimeOpen"))));
    m_configuration.save();
    gtk_widget_hide(m_gobj);
}

void PreferencesDialog::onRowIsFirstTimeOpenActivate()
{
    gtk_switch_set_active(GTK_SWITCH(gtk_builder_get_object(m_builder, "gtk_switchIsFirstTimeOpen")), !gtk_switch_get_active(GTK_SWITCH(gtk_builder_get_object(m_builder, "gtk_switchIsFirstTimeOpen"))));
}
