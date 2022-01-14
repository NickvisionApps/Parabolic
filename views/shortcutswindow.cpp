#include "shortcutswindow.h"

namespace NickvisionTubeConverter::Views
{
    ShortcutsWindow::ShortcutsWindow(Gtk::Window& parent)
    {
        //==Settings==//
        set_title("Shortcuts");
        set_transient_for(parent);
        set_modal(true);
        set_hide_on_close(true);
        //==Converter==//
        m_grpConverter.property_title().set_value("Converter");
        m_shortSelectSaveFolder.property_accelerator().set_value("<Ctrl>O");
        m_shortSelectSaveFolder.property_title().set_value("Select Save Folder");
        m_shortDownloadVideos.property_accelerator().set_value("<Ctrl>D");
        m_shortDownloadVideos.property_title().set_value("Download Queued Videos");
        m_grpConverter.append(m_shortSelectSaveFolder);
        m_grpConverter.append(m_shortDownloadVideos);
        m_section.append(m_grpConverter);
        //==Queue==//
        m_grpQueue.property_title().set_value("Queue");
        m_shortAddToQueue.property_accelerator().set_value("<Ctrl><Shift>A");
        m_shortAddToQueue.property_title().set_value("Add Download to Queue");
        m_shortRemoveFromQueue.property_accelerator().set_value("Delete");
        m_shortRemoveFromQueue.property_title().set_value("Remove Download from Queue");
        m_shortClearQueue.property_accelerator().set_value("<Ctrl><Shift>C");
        m_shortClearQueue.property_title().set_value("Clear Downloads Queue");
        m_grpQueue.append(m_shortAddToQueue);
        m_grpQueue.append(m_shortRemoveFromQueue);
        m_grpQueue.append(m_shortClearQueue);
        m_section.append(m_grpQueue);
        //==Application==//
        m_grpApplication.property_title().set_value("Application");
        m_shortAbout.property_accelerator().set_value("F1");
        m_shortAbout.property_title().set_value("About");
        m_grpApplication.append(m_shortAbout);
        m_section.append(m_grpApplication);
        //==Layout==//
        set_child(m_section);
    }
}
