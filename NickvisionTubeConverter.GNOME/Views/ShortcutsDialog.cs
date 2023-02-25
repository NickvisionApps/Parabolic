using NickvisionTubeConverter.Shared.Helpers;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The ShortcutsDialog for the application
/// </summary>
public class ShortcutsDialog
{
    private readonly Gtk.Builder _builder;
    private readonly Gtk.ShortcutsWindow _window;

    /// <summary>
    /// Constructs a ShortcutDialog
    /// </summary>
    /// <param name="localizer">The application Localizer object</param>
    /// <param name="appName">The application short name</param>
    /// <param name="parent">Gtk.Window</param>
    public ShortcutsDialog(Localizer localizer, string appName, Gtk.Window parent)
    {
        string xml = $@"<?xml version='1.0' encoding='UTF-8'?>
            <interface>
                <object class='GtkShortcutsWindow' id='dialog'>
                    <property name='default-width'>600</property>
                    <property name='default-height'>500</property>
                    <property name='modal'>true</property>
                    <property name='resizable'>true</property>
                    <property name='destroy-with-parent'>false</property>
                    <property name='hide-on-close'>true</property>
                    <child>
                        <object class='GtkShortcutsSection'>
                            <child>
                                <object class='GtkShortcutsGroup'>
                                    <property name='title'>{localizer["Download"]}</property>
                                    <child>
                                        <object class='GtkShortcutsShortcut'>
                                            <property name='title'>{localizer["AddDownload"]}</property>
                                            <property name='accelerator'>&lt;Control&gt;n</property>
                                        </object>
                                    </child>
                                </object>
                            </child>
                            <child>
                                <object class='GtkShortcutsGroup'>
                                    <property name='title'>{localizer["Application", "Shortcut"]}</property>
                                    <child>
                                        <object class='GtkShortcutsShortcut'>
                                            <property name='title'>{localizer["Preferences"]}</property>
                                            <property name='accelerator'>&lt;Control&gt;comma</property>
                                        </object>
                                    </child>
                                    <child>
                                        <object class='GtkShortcutsShortcut'>
                                            <property name='title'>{localizer["KeyboardShortcuts"]}</property>
                                            <property name='accelerator'>&lt;Control&gt;question</property>
                                        </object>
                                    </child>
                                    <child>
                                        <object class='GtkShortcutsShortcut'>
                                            <property name='title'>{string.Format(localizer["About"], appName)}</property>
                                            <property name='accelerator'>F1</property>
                                        </object>
                                    </child>
                                    <child>
                                        <object class='GtkShortcutsShortcut'>
                                            <property name='title'>{localizer["Quit"]}</property>
                                            <property name='accelerator'>&lt;Control&gt;q</property>
                                        </object>
                                    </child>
                                </object>
                            </child>
                        </object>
                    </child>
                </object>
            </interface>";

        _builder = Gtk.Builder.NewFromString(xml, -1);
        _window = (Gtk.ShortcutsWindow)_builder.GetObject("dialog")!;
        _window.SetTransientFor(parent);
    }

    public void Show() => _window.Show();
}
