#ifndef WINUI_H
#define WINUI_H

#include "includes.h"

namespace WinUIHelpers
{
    /**
     * @brief Looks up a WinUI resource for the application.
     * @tparam The type to cast the resource to
     * @param key The key for the resource
     * @returns The T object of the resource
     */
    template<typename T>
    T LookupAppResource(const winrt::hstring& key)
    {
        winrt::Windows::Foundation::IInspectable res{ winrt::Microsoft::UI::Xaml::Application::Current().Resources().Lookup(winrt::box_value(key)) };
        return winrt::unbox_value<T>(res);
    } 
}

#endif //WINUI_H