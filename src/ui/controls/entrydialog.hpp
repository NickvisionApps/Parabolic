#pragma once

#include <string>
#include <adwaita.h>

namespace NickvisionTubeConverter::UI::Controls
{
    /**
     * A dialog for a single line entry
     */
    class EntryDialog
    {
    public:
    	/**
    	 * Constructs a EntryDialog
    	 *
    	 * @param parent The parent window for the dialog
    	 * @param title The title of the dialog
    	 * @param description The description of the choices
    	 * @param entryTitle The title of the entry
    	 */
    	EntryDialog(GtkWindow* parent, const std::string& title, const std::string& description, const std::string& entryTitle);
		/**
    	 * Gets the GtkWidget* representing the EntryDialog
    	 *
    	 * @returns The GtkWidget* representing the EntryDialog
    	 */
    	GtkWidget* gobj();
    	/**
    	 * Run the EntryDialog
    	 *
    	 * @returns The string from the entry
    	 */
    	std::string run();

    private:
    	std::string m_response;
    	GtkWidget* m_gobj{ nullptr };
    	GtkWidget* m_preferencesGroup{ nullptr };
		GtkWidget* m_rowEntry{ nullptr };
		void setResponse(const std::string& response);
    };
}