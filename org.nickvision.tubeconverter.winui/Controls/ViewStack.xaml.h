#ifndef VIEWSTACK_H
#define VIEWSTACK_H

#include "pch.h"
#include "Controls/ViewStack.g.h"

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::implementation
{
    /**
     * @brief A control used for storing pages and showing one at a time.
     */
    struct ViewStack : public ViewStackT<ViewStack>
    {
    public:
        /**
         * @brief Constructs a ViewStack.
         */
        ViewStack();
        /**
         * @brief Gets the view stack's pages dependency property.
         * @return The pages dependency property
         */
        static const Microsoft::UI::Xaml::DependencyProperty& PagesProperty();
        /**
         * @brief Gets the index of the current page being shown.
         * @return The index of the currently shown page
         */
        int CurrentPageIndex() const;
        /**
         * @brief Sets the index of the new page to show.
         * @param currentPage The index of the new page to show
         */
        void CurrentPageIndex(int index);
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

    private:
        static Microsoft::UI::Xaml::DependencyProperty m_pagesProperty;
        int m_currentPageIndex;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::factory_implementation
{
    struct ViewStack : public ViewStackT<ViewStack, implementation::ViewStack> { };
}

#endif //VIEWSTACK_H
