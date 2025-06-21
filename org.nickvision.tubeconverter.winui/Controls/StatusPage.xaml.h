#ifndef STATUSPAGE_H
#define STATUSPAGE_H

#include "pch.h"
#include "Controls/StatusPage.g.h"

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::implementation
{
    /**
     * @brief A page control to display a status of the application.
     */
    struct StatusPage : public StatusPageT<StatusPage>
    {
    public:
        /**
         * @brief Constructs a StatusPage.
         */
        StatusPage();
        /**
         * @brief Gets the page's glyph dependency property.
         * @return The glyph dependency property
         */
        static const Microsoft::UI::Xaml::DependencyProperty& GlyphProperty();
        /**
         * @brief Gets the page's use app icon dependency property.
         * @return The use app icon dependency property
         */
        static const Microsoft::UI::Xaml::DependencyProperty& UseAppIconProperty();
        /**
         * @brief Gets the page's title dependency property.
         * @return The title dependency property
         */
        static const Microsoft::UI::Xaml::DependencyProperty& TitleProperty();
        /**
         * @brief Gets the page's description dependency property.
         * @return The description dependency property
         */
        static const Microsoft::UI::Xaml::DependencyProperty& DescriptionProperty();
        /**
         * @brief Gets the page's child dependency property.
         * @return The child dependency property
         */
        static const Microsoft::UI::Xaml::DependencyProperty& ChildProperty();
        /**
         * @brief Gets the page's is compact dependency property.
         * @return The is compact dependency property
         */
        static const Microsoft::UI::Xaml::DependencyProperty& IsCompactProperty();
        /**
         * @brief Handles when a property of the row is changed.
         * @param d Microsoft::UI::Xaml::DependencyObject
         * @param args Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs
         */
        static void OnPropertyChanged(const Microsoft::UI::Xaml::DependencyObject& d, const Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs& args);
        /**
         * @brief Gets the icon glyph for the page.
         * @brief This will be used when UseAppIcon is set to false.
         * @return The page icon glyph
         */
        winrt::hstring Glyph() const;
        /**
         * @brief Sets the icon glyph for the page.
         * @brief To be used when UseAppIcon is set to false.
         * @param glyph The new icon glyph
         */
        void Glyph(const winrt::hstring& glyph);
        /**
         * @brief Gets whether or not the page is displaying the application's icon.
         * @brief If false, the Glyph icon is being used instead.
         * @return True if displaying app icon, else false
         */
        bool UseAppIcon() const;
        /**
         * @brief Sets whether or not the page should display the application's icon.
         * @param useAppIcon True to display app icon. False to use the Glyph icon
         */
        void UseAppIcon(bool useAppIcon);
        /**
         * @brief Gets the status title for the page.
         * @return The status title
         */
        winrt::hstring Title() const;
        /**
         * @brief Sets the status title for the page.
         * @param title The new status title
         */
        void Title(const winrt::hstring& title);
        /**
         * @brief Gets the status description for the page.
         * @return The status description
         */
        winrt::hstring Description() const;
        /**
         * @brief Sets the status title for the page.
         * @param title The new status title
         */
        void Description(const winrt::hstring& description);
        /**
         * @brief Gets the extra child for the page.
         * @return The extra child
         */
        IInspectable Child() const;
        /**
         * @brief Sets the extra child for the page.
         * @param title The new extra child
         */
        void Child(const IInspectable& child);
        /**
         * @brief Gets whether or not the page is displayed using compact spacing.
         * @return True for compact spacing, else false
         */
        bool IsCompact() const;
        /**
         * @brief Sets whether or not the page should use compact spacing.
         * @param isCompact True for compact spacing, else false
         */
        void IsCompact(bool isCompact);
        /**
         * @brief Gets the visibility of the glyph icon.
         * @return The visibility of the glyph icon
         */
        Microsoft::UI::Xaml::Visibility GlyphIconVisibility() const;
        /**
         * @brief Gets the visibility of the app icon.
         * @return The visibility of the app icon
         */
        Microsoft::UI::Xaml::Visibility AppIconVisibility() const;
        /**
         * @brief Gets the visibility of the title control.
         * @return The visibility of the title control
         */
        Microsoft::UI::Xaml::Visibility TitleVisibility() const;
        /**
         * @brief Gets the visibility of the description control.
         * @return The visibility of the description control
         */
        Microsoft::UI::Xaml::Visibility DescriptionVisibility() const;
        /**
         * @brief Gets the visibility of the child control.
         * @return The visibility of the child control
         */
        Microsoft::UI::Xaml::Visibility ChildVisibility() const;
        /**
         * @brief Gets the spacing between the controls.
         * @return The spacing between the controls
         */
        double CompactSpacing() const;
        /**
         * @brief Gets the font size of the icon.
         * @return The font size of the icon
         */
        double IconSize() const;
        /**
         * @brief Gets the width of the icon.
         * @return The width of the icon
         */
        double IconWidth() const;
        /**
         * @brief Gets the height of the icon.
         * @return The height of the icon
         */
        double IconHeight() const;
        /**
         * @brief Gets the style of the title control.
         * @return The style of the title control
         */
        Microsoft::UI::Xaml::Style TitleStyle() const;
        /**
         * @brief Subscribes a handler to the property changed event.
         * @return The token for the newly subscribed handler.
         */
        winrt::event_token PropertyChanged(const Microsoft::UI::Xaml::Data::PropertyChangedEventHandler& handler);
        /**
         * @brief Unsubscribes a handler from the property changed event.
         * @param token The token of the handler to unsubscribe.
         */
        void PropertyChanged(const winrt::event_token& token);

    private:
        static Microsoft::UI::Xaml::DependencyProperty m_glyphProperty;
        static Microsoft::UI::Xaml::DependencyProperty m_useAppIconProperty;
        static Microsoft::UI::Xaml::DependencyProperty m_titleProperty;
        static Microsoft::UI::Xaml::DependencyProperty m_descriptionProperty;
        static Microsoft::UI::Xaml::DependencyProperty m_childProperty;
        static Microsoft::UI::Xaml::DependencyProperty m_isCompactProperty;
        winrt::event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::factory_implementation
{
    struct StatusPage : public StatusPageT<StatusPage, implementation::StatusPage> { };
}

#endif //STATUSPAGE_H
