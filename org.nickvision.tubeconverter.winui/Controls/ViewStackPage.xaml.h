#ifndef VIEWSTACKPAGE_H
#define VIEWSTACKPAGE_H

#include "includes.h"
#include "Controls/ViewStackPage.g.h"

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::implementation 
{
    /**
     * @brief A page control to be stored in a ViewStack. 
     */
    class ViewStackPage : public ViewStackPageT<ViewStackPage>
    {
    public:
        /**
         * @brief Constructs a ViewStackPage. 
         */
        ViewStackPage();
        /**
         * @brief Gets the name of this page.
         * @returns The page's name 
         */
        winrt::hstring PageName() const;
        /**
         * @brief Sets the name of this page.
         * @param pageName The new name of the page 
         */
        void PageName(const winrt::hstring& pageName);

    public:
        /**
         * @brief Gets the page's page name dependency property
         * @return The page name dependency property 
         */
        static const Microsoft::UI::Xaml::DependencyProperty& PageNameProperty();

    private:
        static Microsoft::UI::Xaml::DependencyProperty m_pageNameProperty;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::factory_implementation 
{
    class ViewStackPage : public ViewStackPageT<ViewStackPage, implementation::ViewStackPage>
    {

    };
}

#endif //VIEWSTACKPAGE_H