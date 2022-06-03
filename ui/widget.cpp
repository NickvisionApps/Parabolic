#include "widget.h"

using namespace NickvisionTubeConverter::UI;

Widget::Widget(const std::string& resourcePath, const std::string& mainWidgetName) : m_builder{gtk_builder_new_from_resource(resourcePath.c_str())}, m_gobj{GTK_WIDGET(gtk_builder_get_object(m_builder, mainWidgetName.c_str()))}
{

}

GtkBuilder* Widget::getBuilder()
{
    return m_builder;
}

GtkWidget* Widget::gobj()
{
    return m_gobj;
}

void Widget::show()
{
    gtk_widget_show(m_gobj);
}
