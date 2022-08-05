#include "CurlHelpers.h"
#include <fstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

namespace NickvisionTubeConverter::Helpers
{
    bool CurlHelpers::downloadFile(const std::string& url, const std::string& path)
    {
        std::ofstream fileOut{ path };
        if (fileOut.is_open())
        {
            cURLpp::Cleanup cleanup;
            cURLpp::Easy handle;
            try
            {
                handle.setOpt(cURLpp::Options::Url(url));
                handle.setOpt(cURLpp::Options::FollowLocation(true));
                handle.setOpt(cURLpp::Options::WriteStream(&fileOut));
                handle.perform();
            }
            catch (...)
            {
                return false;
            }
            return true;
        }
        return false;
    }
}