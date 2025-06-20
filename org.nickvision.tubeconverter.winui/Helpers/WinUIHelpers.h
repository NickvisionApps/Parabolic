#ifndef WINUIHELPERS_H
#define WINUIHELPERS_H

#include "pch.h"

namespace Nickvision::TubeConverter::WinUI::Helpers::WinUIHelpers
{
    /**
     * @brief Looks up a WinUI resource for the application.
     * @tparam The type to cast the resource to
     * @param key The key for the resource
     * @return The T object of the resource
     */
    template<typename T>
    T LookupAppResource(const winrt::hstring& key)
    {
        winrt::Windows::Foundation::IInspectable res{ winrt::Microsoft::UI::Xaml::Application::Current().Resources().Lookup(winrt::box_value(key)) };
        return winrt::unbox_value<T>(res);
    }
}

#endif //WINUIHELPERS_H
