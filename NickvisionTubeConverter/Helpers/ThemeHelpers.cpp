#include "ThemeHelpers.h"
#include <dwmapi.h>
#include "../Models/Configuration.h"

using namespace NickvisionTubeConverter::Models;

namespace NickvisionTubeConverter::Helpers
{
    QPalette ThemeHelpers::getThemedPalette()
    {
        QPalette palette;
        if (Configuration::getInstance().getTheme() == Theme::Light)
        {
            palette.setColor(QPalette::Window, QColor(255, 255, 255));
            palette.setColor(QPalette::WindowText, Qt::black);
            palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
            palette.setColor(QPalette::Base, QColor(250, 250, 250));
            palette.setColor(QPalette::AlternateBase, QColor(243, 243, 243));
            palette.setColor(QPalette::ToolTipBase, Qt::black);
            palette.setColor(QPalette::ToolTipText, Qt::black);
            palette.setColor(QPalette::Text, Qt::black);
            palette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
            palette.setColor(QPalette::Button, QColor(243, 243, 243));
            palette.setColor(QPalette::ButtonText, Qt::black);
            palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
            palette.setColor(QPalette::BrightText, Qt::red);
            palette.setColor(QPalette::Link, QColor(42, 130, 218));
            palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
            palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
            palette.setColor(QPalette::HighlightedText, Qt::white);
            palette.setColor(QPalette::PlaceholderText, Qt::darkGray);
            palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
        }
        else
        {
            palette.setColor(QPalette::Window, QColor(25, 25, 25));
            palette.setColor(QPalette::WindowText, Qt::white);
            palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
            palette.setColor(QPalette::Base, QColor(43, 43, 43));
            palette.setColor(QPalette::AlternateBase, QColor(55, 55, 55));
            palette.setColor(QPalette::ToolTipBase, Qt::white);
            palette.setColor(QPalette::ToolTipText, Qt::white);
            palette.setColor(QPalette::Text, Qt::white);
            palette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
            palette.setColor(QPalette::Button, QColor(43, 43, 43));
            palette.setColor(QPalette::ButtonText, Qt::white);
            palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
            palette.setColor(QPalette::BrightText, Qt::red);
            palette.setColor(QPalette::Link, QColor(42, 130, 218));
            palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
            palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
            palette.setColor(QPalette::HighlightedText, Qt::white);
            palette.setColor(QPalette::PlaceholderText, Qt::gray);
            palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
        }
        return palette;
    }

    QString ThemeHelpers::getThemedSeparatorStyle()
    {
        if (Configuration::getInstance().getTheme() == Theme::Light)
        {
            return "background-color: #c4c2c2;";
        }
        else
        {
            return "background-color: #2b2b2b;";
        }
    }

	void ThemeHelpers::applyWin32Theme(QWidget* widget)
	{
        BOOL isDarkMode{ Configuration::getInstance().getTheme() == Theme::Light ? FALSE : TRUE };
        COLORREF themeColor{ isDarkMode ? RGB(25, 25, 25) : RGB(255, 255, 255) };
        //DWMWA_USE_IMMERSIVE_DARK_MODE 
		DwmSetWindowAttribute((HWND)widget->winId(), 20, &isDarkMode, sizeof(isDarkMode));
        //DWMWA_BORDER_COLOR
        DwmSetWindowAttribute((HWND)widget->winId(), 34, &themeColor, sizeof(themeColor));
        //DWMWA_CAPTION_COLOR
        DwmSetWindowAttribute((HWND)widget->winId(), 35, &themeColor, sizeof(themeColor));
	}
}