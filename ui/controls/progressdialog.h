#pragma once

#include <string>
#include <thread>
#include <functional>
#include <mutex>
#include <adwaita.h>
#include "../widget.h"

namespace NickvisionTubeConverter::UI::Controls
{
    class ProgressDialog : public NickvisionTubeConverter::UI::Widget
    {
    public:
        ProgressDialog(GtkWidget* parent, const std::string& description, const std::function<void()>& work, const std::function<void()>& then);
        void show() override;

    private:
        mutable std::mutex m_mutex;
        std::function<void()> m_work;
        std::function<void()> m_then;
        bool m_isFinished;
        std::jthread m_thread;
        //==Singals==//
        bool timeout();
    };
}
