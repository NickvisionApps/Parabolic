#pragma once

#include <string>

namespace NickvisionTubeConverter::Helpers::CurlHelpers
{
    bool downloadFile(const std::string& url, const std::string& path);
}