#ifndef SHORTCUTSWINDOW_H
#define SHORTCUTSWINDOW_H

#include <gtkmm.h>

namespace NickvisionTubeConverter::Views
{
    class ShortcutsWindow : public Gtk::ShortcutsWindow
    {
    public:
        ShortcutsWindow(Gtk::Window& parent);

    private:
        Gtk::ShortcutsSection m_section;
        Gtk::ShortcutsGroup m_grpConverter;
        Gtk::ShortcutsShortcut m_shortSelectSaveFolder;
        Gtk::ShortcutsShortcut m_shortDownloadVideos;
        Gtk::ShortcutsGroup m_grpQueue;
        Gtk::ShortcutsShortcut m_shortAddToQueue;
        Gtk::ShortcutsShortcut m_shortRemoveFromQueue;
        Gtk::ShortcutsShortcut m_shortClearQueue;
        Gtk::ShortcutsGroup m_grpApplication;
        Gtk::ShortcutsShortcut m_shortAbout;
    };
}

#endif // SHORTCUTSWINDOW_H
