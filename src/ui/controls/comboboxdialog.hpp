#pragma once

#include <string>
#include <vector>
#include <adwaita.h>

namespace NickvisionTubeConverter::UI::Controls
{
	/**
	 * A dialog for selecting a choice from a list of choices
	 */
	class ComboBoxDialog
	{
	public:
		/**
		 * Constructs a ComboBoxDialog
		 *
		 * @param parent The parent window for the dialog
		 * @param title The title of the dialog
		 * @param description The description of the choices
		 * @param rowTitle The title of the choices
		 * @param choices The list of choices
		 */
		ComboBoxDialog(GtkWindow* parent, const std::string& title, const std::string& description, const std::string& rowTitle, const std::vector<std::string>& choices);
		/**
		 * Gets the GtkWidget* representing the ComboBoxDialog
		 *
		 * @returns The GtkWidget* representing the ComboBoxDialog
		 */
		GtkWidget* gobj();
		/**
		 * Run the ComboBoxDialog
		 *
		 * @returns The selected choice from the combo box
		 */
		std::string run();

	private:
		std::vector<std::string> m_choices;
		std::string m_response;
		GtkWidget* m_gobj{ nullptr };
		GtkWidget* m_preferencesGroup{ nullptr };
		GtkWidget* m_rowChoices{ nullptr };
		void setResponse(const std::string& response);
	};
}