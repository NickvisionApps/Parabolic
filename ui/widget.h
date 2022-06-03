#pragma once

#include <string>
#include <adwaita.h>

namespace NickvisionTubeConverter::UI
{
    class Widget
    {
    public:
        Widget(const std::string& resourcePath, const std::string& mainWidgetName);
        virtual ~Widget() = default;
        GtkBuilder* getBuilder();
        GtkWidget* gobj();
        virtual void show();

    protected:
        GtkBuilder* m_builder;
        GtkWidget* m_gobj;
    };
}
