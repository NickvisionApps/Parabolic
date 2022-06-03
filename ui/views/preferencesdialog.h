#pragma once

#include <adwaita.h>
#include "../widget.h"
#include "../../models/configuration.h"

namespace NickvisionTubeConverter::UI::Views
{
    class PreferencesDialog : public NickvisionTubeConverter::UI::Widget
    {
    public:
        PreferencesDialog(GtkWidget* parent, NickvisionTubeConverter::Models::Configuration& configuration);
        ~PreferencesDialog();

    private:
        NickvisionTubeConverter::Models::Configuration& m_configuration;
        //==Signals==//
        void cancel();
        void save();
        void onRowIsFirstTimeOpenActivate();
    };
}
