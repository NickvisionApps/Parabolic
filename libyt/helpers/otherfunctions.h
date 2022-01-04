#ifndef OTHERFUNCTIONS_H
#define OTHERFUNCTIONS_H

#include <string>
#include <vector>

namespace NickvisionTubeConverter::libyt::Helpers
{
    int skipWhitespace(const std::string& text, int start);
    std::vector<std::string> getUrisFromManifest(const std::string& source);
    std::string jsonExtract(const std::string& source);
    std::pair<bool, std::string> jsonGetKey(const std::string& key, const std::string& source);
}

#endif // OTHERFUNCTIONS_H
