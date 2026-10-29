#include "download.h"
#include <algorithm>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

namespace parabolic::download
{
    int Options::max_number_of_active_downloads() const
    {
        return m_max_number_of_active_downloads;
    }

    void Options::set_max_number_of_active_downloads(int value)
    {
        m_max_number_of_active_downloads = value;
    }

    bool Options::overwrite_existing_files() const
    {
        return m_overwrite_existing_files;
    }

    void Options::set_overwrite_existing_files(bool value)
    {
        m_overwrite_existing_files = value;
    }

    bool Options::prevent_suspend() const
    {
        return m_prevent_suspend;
    }

    void Options::set_prevent_suspend(bool value)
    {
        m_prevent_suspend = value;
    }

    const Options& Manager::options() const
    {
        return m_options;
    }

    void Manager::set_options(const Options& options)
    {
        m_options = options;
    }

    void from_json(const nlohmann::json& json, Options& options)
    {
        if (json.contains("maxNumberOfActiveDownloads") && json["maxNumberOfActiveDownloads"].is_number_integer())
        {
            options.m_max_number_of_active_downloads =
                    std::clamp(json.at("maxNumberOfActiveDownloads").get<int>(), 1, 10);
        }
        else if (json.contains("MaxNumberOfActiveDownloads") && json["MaxNumberOfActiveDownloads"].is_number_integer())
        {
            options.m_max_number_of_active_downloads =
                    std::clamp(json.at("MaxNumberOfActiveDownloads").get<int>(), 1, 10);
        }
        if (json.contains("overwriteExistingFiles") && json["overwriteExistingFiles"].is_boolean())
        {
            options.m_overwrite_existing_files = json.at("overwriteExistingFiles").get<bool>();
        }
        else if (json.contains("OverwriteExistingFiles") && json["OverwriteExistingFiles"].is_boolean())
        {
            options.m_overwrite_existing_files = json.at("OverwriteExistingFiles").get<bool>();
        }
        if (json.contains("preventSuspend") && json["preventSuspend"].is_boolean())
        {
            options.m_prevent_suspend = json.at("preventSuspend").get<bool>();
        }
        else if (json.contains("PreventSuspend") && json["PreventSuspend"].is_boolean())
        {
            options.m_prevent_suspend = json.at("PreventSuspend").get<bool>();
        }
    }

    void to_json(nlohmann::json& json, const Options& options)
    {
        json = nlohmann::json{
            { "maxNumberOfActiveDownloads", options.m_max_number_of_active_downloads },
            { "overwriteExistingFiles", options.m_overwrite_existing_files },
            { "preventSuspend", options.m_prevent_suspend },
        };
    }
}
