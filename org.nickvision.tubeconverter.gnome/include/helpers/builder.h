#ifndef BUILDER_H
#define BUILDER_H

#include <string>
#include <adwaita.h>

namespace Nickvision::TubeConverter::GNOME::BuilderHelpers
{
    /**
     * @brief Gets a GtkBuilder object for a compiled blueprint ui file.
     * @brief Compiled blueprint ui files (.ui not .blp) should be stored in the path: "Current_Running_Direction/ui/blueprint_file_name.ui".
     * @param blueprint The name of the blueprint ui file
     * @return The GtkBiilder object for the blueprint file or nullptr on error
     */
    GtkBuilder* fromBlueprint(const std::string& blueprint);
}

#endif //BUILDER_H