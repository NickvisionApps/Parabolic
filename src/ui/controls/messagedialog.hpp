#pragma once

#include <string>
#include <adwaita.h>

namespace NickvisionTubeConverter::UI::Controls
{
	/**
	 * Responses for the MessageDialog
	 */
	enum class MessageDialogResponse
	{
		Suggested,
		Destructive,
		Cancel
	};

    /**
     * A dialog for displaying a message
     */
    class MessageDialog
    {
    public:
    	/**
    	 * Constructs a MessageDialog
    	 *
    	 * @param parent The parent window for the dialog
    	 * @param title The title of the dialog
    	 * @param description The description of the choices
    	 * @param cancelText The text of the cancel button
    	 * @param destructiveText The text of the destructive button
    	 * @param suggestedText The text of the suggested button
    	 */
    	MessageDialog(GtkWindow* parent, const std::string& title, const std::string& description, const std::string& cancelText, const std::string& destructiveText = "", const std::string& suggestedText = "");
		/**
    	 * Gets the GtkWidget* representing the MessageDialog
    	 *
    	 * @returns The GtkWidget* representing the MessageDialog
    	 */
    	GtkWidget* gobj();
    	/**
    	 * Run the MessageDialog
    	 *
    	 * @returns The response from the dialog
    	 */
    	MessageDialogResponse run();

    private:
    	MessageDialogResponse m_response;
    	GtkWidget* m_gobj{ nullptr };
		void setResponse(const std::string& response);
    };
}