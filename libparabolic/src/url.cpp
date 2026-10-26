#include "url.h"
#include <curl/curl.h>
#include <string>

namespace parabolic::url
{
    bool is_valid(const std::string& url)
    {
        CURLU* handle{ curl_url() };
        bool valid{ curl_url_set(handle, CURLUPART_URL, url.c_str(), 0) == CURLUE_OK };
        curl_url_cleanup(handle);
        return valid;
    }
}
