#ifndef FILETYPEHELPERS_H
#define FILETYPEHELPERS_H

#include <string>
#include "../models/filetype.h"

namespace NickvisionTubeConverter::Helpers::FileTypeHelpers
{
    std::string toString(NickvisionTubeConverter::Models::FileType fileType);
    std::string toDotExtension(NickvisionTubeConverter::Models::FileType fileType);
    NickvisionTubeConverter::Models::FileType parse(const std::string& s);
    bool isAudio(NickvisionTubeConverter::Models::FileType fileType);
    bool isVideo(NickvisionTubeConverter::Models::FileType fileType);
}

#endif // FILETYPEHELPERS_H
