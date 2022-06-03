#include "comboboxdialog.h"

using namespace NickvisionTubeConverter::UI;
using namespace NickvisionTubeConverter::UI::Controls;

ComboBoxDialog::ComboBoxDialog(GtkWidget* parent, const std::string& title, const std::string& description, const std::string& rowTitle, const std::vector<std::string>& choices) : Widget{"/org/nickvision/tubeconverter/ui/controls/comboboxdialog.xml", "gtk_comboBoxDialog"}, m_choices{choices}, m_selectedChoice{""}
{
    //==Dialog==//
    gtk_window_set_transient_for(GTK_WINDOW(m_gobj), GTK_WINDOW(parent));
    //==Signals==//
    g_signal_connect(gtk_builder_get_object(m_builder, "gtk_btnCancel"), "clicked", G_CALLBACK((void (*)(GtkButton*, gpointer*))[](GtkButton* button, gpointer* data) { reinterpret_cast<ComboBoxDialog*>(data)->cancel(); }), this);
    g_signal_connect(gtk_builder_get_object(m_builder, "gtk_btnOK"), "clicked", G_CALLBACK((void (*)(GtkButton*, gpointer*))[](GtkButton* button, gpointer* data) { reinterpret_cast<ComboBoxDialog*>(data)->ok(); }), this);
    //==Setup UI==//
    adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(gtk_builder_get_object(m_builder, "adw_group")), title.c_str());
    adw_preferences_group_set_description(ADW_PREFERENCES_GROUP(gtk_builder_get_object(m_builder, "adw_group")), description.c_str());
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(gtk_builder_get_object(m_builder, "adw_row")), rowTitle.c_str());
    GtkStringList* gtkChoices = gtk_string_list_new(nullptr);
    for(const std::string& string : m_choices)
    {
        gtk_string_list_append(gtkChoices, string.c_str());
    }
    adw_combo_row_set_model(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "adw_row")), G_LIST_MODEL(gtkChoices));
}

ComboBoxDialog::~ComboBoxDialog()
{
    gtk_window_destroy(GTK_WINDOW(m_gobj));
}

const std::string& ComboBoxDialog::getSelectedChoice()
{
    return m_selectedChoice;
}

void ComboBoxDialog::cancel()
{
    gtk_widget_hide(m_gobj);
}

void ComboBoxDialog::ok()
{
    unsigned int selectedRow = adw_combo_row_get_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "adw_row")));
    m_selectedChoice = m_choices[selectedRow];
    gtk_widget_hide(m_gobj);
}