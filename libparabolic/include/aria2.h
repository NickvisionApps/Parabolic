#ifndef ARIA2_H
#define ARIA2_H

#include <nlohmann/json_fwd.hpp>

namespace parabolic::aria2
{
    class Options
    {
    public:
        [[nodiscard]] int max_connections_per_server() const;
        void set_max_connections_per_server(int value);
        [[nodiscard]] int min_split_size() const;
        void set_min_split_size(int value);
        [[nodiscard]] bool use_aria() const;
        void set_use_aria(bool value);
        bool operator==(const Options& rhs) const = default;
        friend void from_json(const nlohmann::json& json, Options& options);
        friend void to_json(nlohmann::json& json, const Options& options);

    private:
        int m_max_connections_per_server{ 16 };
        int m_min_split_size{ 20 };
        bool m_use_aria{ false };
    };
}

#endif
