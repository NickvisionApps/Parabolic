#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <nlohmann/json_fwd.hpp>

namespace parabolic::download
{
    class Options
    {
    public:
        [[nodiscard]] int max_number_of_active_downloads() const;
        void set_max_number_of_active_downloads(int value);
        [[nodiscard]] bool overwrite_existing_files() const;
        void set_overwrite_existing_files(bool value);
        [[nodiscard]] bool prevent_suspend() const;
        void set_prevent_suspend(bool value);
        bool operator==(const Options& rhs) const = default;
        friend void from_json(const nlohmann::json& json, Options& options);
        friend void to_json(nlohmann::json& json, const Options& options);

    private:
        int m_max_number_of_active_downloads{ 5 };
        bool m_overwrite_existing_files{ true };
        bool m_prevent_suspend{ false };
    };

    class Manager
    {
    public:
        Manager() = default;
        [[nodiscard]] const Options& options() const;
        void set_options(const Options& options);

    private:
        Options m_options;
    };
}

#endif
