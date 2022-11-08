#pragma once

#include <string>
#include <adwaita.h>
#include "messagedialog.hpp"

namespace NickvisionTubeConverter::UI::Controls
{
	class LogsDialog : public MessageDialog
	{
	public:
		/**
    	 * Constructs a LogsDialog
    	 *
    	 * @param parent The parent window for the dialog
    	 * @param title The title of the dialog
    	 * @param description The description of the choices
    	 * @param cancelText The text of the cancel button
    	 * @param destructiveText The text of the destructive button
    	 * @param suggestedText The text of the suggested button
    	 */
		LogsDialog(GtkWindow* parent, const std::string& title, const std::string& description, const std::string& cancelText, const std::string& destructiveText = "", const std::string& suggestedText = "");

	private:
		GtkWidget* m_scrolledWindow;
		GtkWidget* m_textView;
		GtkTextBuffer* m_textBuffer;
	};
}