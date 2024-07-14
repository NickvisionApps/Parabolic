#ifndef VIEWSTACK_H
#define VIEWSTACK_H

#include "includes.h"
#include "Controls/ViewStack.g.h"

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::implementation 
{
    /**
     * @brief A control used for storing pages and showing one at a time. 
     */
    class ViewStack : public ViewStackT<ViewStack>
    {
    public:
        /**
         * @brief Constructs a ViewStack. 
         */
        ViewStack();
        /**
         * @brief Gets the name of the current page being shown.
         * @return The name of the currently shown page 
         */
        winrt::hstring CurrentPage() const;
        /**
         * @brief Sets the name of the new page to show.
         * @param currentPage The name of the new page to show
         */
        void CurrentPage(const winrt::hstring& currentPage);
        /**
         * @brief Gets the list of pages being stored in the view stack.
         * @return The list of stored page objects 
         */
        Windows::Foundation::Collections::IObservableVector<IInspectable> Pages() const;
        /**
         * @brief Sets the list of pages to store in the view stack.
         * @param value The list of pages to store
         */
        void Pages(const Windows::Foundation::Collections::IObservableVector<IInspectable>& value);
        /**
         * @brief Subscribes a handler to the page changed event.
         * @return The token for the newly subscribed handler.
         */
        winrt::event_token PageChanged(const Windows::Foundation::EventHandler<winrt::hstring>& handler);
        /**
         * @brief Unsubscribes a handler from the page changed event.
         * @param token The token of the handler to unsubscribe. 
         */
        void PageChanged(const winrt::event_token& token);

    private:
        winrt::event<Windows::Foundation::EventHandler<winrt::hstring>> m_pageChangedEvent;
        winrt::hstring m_currentPage;

    public:
        /**
         * @brief Gets the view stack's pages dependency property.
         * @return The pages dependency property 
         */
        static const Microsoft::UI::Xaml::DependencyProperty& PagesProperty();

    private:
        static Microsoft::UI::Xaml::DependencyProperty m_pagesProperty;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::factory_implementation 
{
    class ViewStack : public ViewStackT<ViewStack, implementation::ViewStack>
    {

    };
}

#endif //VIEWSTACK_H