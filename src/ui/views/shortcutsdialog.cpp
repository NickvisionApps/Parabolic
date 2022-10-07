#include "shortcutsdialog.hpp"

using namespace NickvisionTubeConverter::UI::Views;

ShortcutsDialog::ShortcutsDialog(GtkWindow* parent)
{
    m_xml = R"(
    <?xml version="1.0" encoding="UTF-8"?>
    <interface>
        <object class="GtkShortcutsWindow" id="m_dialog">
            <property name="default-width">600</property>
            <property name="default-height">500</property>
            <property name="modal">true</property>
            <property name="resizable">true</property>
            <property name="destroy-with-parent">false</property>
            <property name="hide-on-close">true</property>
            <child>
                <object class="GtkShortcutsSection">
                    <child>
                        <object class="GtkShortcutsGroup">
                            <property name="title">Application</property>
                            <child>
                                <object class="GtkShortcutsShortcut">
                                    <property name="title">Preferences</property>
                                    <property name="accelerator">&lt;Control&gt;comma</property>
                                </object>
                            </child>
                            <child>
                                <object class="GtkShortcutsShortcut">
                                    <property name="title">Keyboard Shortcuts</property>
                                    <property name="accelerator">&lt;Control&gt;question</property>
                                </object>
                            </child>
                            <child>
                                <object class="GtkShortcutsShortcut">
                                    <property name="title">About</property>
                                    <property name="accelerator">F1</property>
                                </object>
                            </child>
                        </object>
                    </child>
                </object>
            </child>
        </object>
    </interface>
    )";
    GtkBuilder* builder{ gtk_builder_new_from_string(m_xml.c_str(), -1) };
    m_gobj = GTK_WIDGET(gtk_builder_get_object(builder, "m_dialog"));
    gtk_window_set_transient_for(GTK_WINDOW(m_gobj), GTK_WINDOW(parent));
}

GtkWidget* ShortcutsDialog::gobj()
{
    return m_gobj;
}

void ShortcutsDialog::run()
{
    gtk_widget_show(m_gobj);
    while(gtk_widget_is_visible(m_gobj))
    {
        g_main_context_iteration(g_main_context_default(), false);
    }
    gtk_window_destroy(GTK_WINDOW(m_gobj));
}
