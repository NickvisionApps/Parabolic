#include "otherfunctions.h"
#include <stdexcept>
#include <sstream>

namespace NickvisionTubeConverter::libyt::Helpers
{
    int skipWhitespace(const std::string& text, int start)
    {
        int result = start;
        while(isspace(text[result]))
        {
            result++;
        }
        return result;
    }

    std::vector<std::string> getUrisFromManifest(const std::string& source)
    {
        std::vector<std::string> uris;
        std::string opening = "<BaseURL";
        std::string closing = "</BaseURL>";
        size_t start = source.find(opening);
        if(start != std::string::npos)
        {
            std::vector<std::string> tokens;
            size_t posDelimiter;
            std::string temp = source.substr(start);
            while((posDelimiter = temp.find(opening)) != std::string::npos)
            {
                std::string token = temp.substr(0, posDelimiter);
                if(!token.empty())
                {
                    tokens.push_back(token);
                }
                temp.erase(0, posDelimiter + opening.length());
            }
            for(const std::string& token : tokens)
            {
                uris.push_back(token.substr(0, token.find(closing)));
            }
            return uris;
        }
        throw std::invalid_argument("Unable to get URIs from source.");
    }

    std::string jsonExtract(const std::string& source)
    {
        std::stringstream builder;
        int depth = 0;
        char lastChar = '\u0000';
        for(char ch : source)
        {
            builder << ch;
            if (ch == '{' && lastChar != '\\')
            {
                depth++;
            }
            else if (ch == '}' && lastChar != '\\')
            {
                depth--;
            }
            if (depth == 0)
            {
                break;
            }
            lastChar = ch;
        }
        return builder.str();
    }

    std::pair<bool, std::string> jsonGetKey(const std::string& key, const std::string& source)
    {
        std::string quotedKey = '"' + key + '"';
        size_t index = 0;
        while(true)
        {
            index = source.find(quotedKey, index);
            if (index == std::string::npos)
            {
                return std::make_pair<bool, std::string>(false, "");
            }
            index += quotedKey.size();
            size_t start = index;
            start = skipWhitespace(source, start);
            if (source[start++] != ':')
            {
                continue;
            }
            start = skipWhitespace(source, start);
            if (source[start++] != '"')
            {
                continue;
            }
            size_t end = start;
            while ((source[end - 1] == '\\' && source[end] == '"') || source[end] != '"')
            {
                end++;
            }
            return std::make_pair<bool, std::string>(true, source.substr(start, end - start));
        }
    }
}
