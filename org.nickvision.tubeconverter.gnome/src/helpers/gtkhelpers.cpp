#include "helpers/gtkhelpers.h"
#include <adwaita.h>

namespace Nickvision::TubeConverter::GNOME::Helpers
{
    void GtkHelpers::dispatchToMainThread(const std::function<void()>& func)
    {
        g_main_context_invoke(g_main_context_default(), +[](gpointer data) -> int
        {
            std::function<void()>* function{ static_cast<std::function<void()>*>(data) };
            (*function)();
            delete function;
            return false;
        }, new std::function<void()>(func));
    }
}