#ifndef YTDLP_H
#define YTDLP_H

#include <expected>
#include <functional>
#include <optional>
#include <stop_token>
#include <string>
#include <vector>
#include "discovery.h"
#include "extractor.h"
#include "keyring.h"

namespace parabolic::ytdlp
{
    using ProcessOutputCallback = std::function<void(const std::string&)>;

    class Options
    {
    public:
        Options(const std::string& url, ProcessOutputCallback output_callback = {}, std::stop_token stop_token = {});
        [[nodiscard]] const std::vector<std::string>& arguments() const;
        [[nodiscard]] const ProcessOutputCallback& output_callback() const;
        [[nodiscard]] const std::stop_token& stop_token() const;
        void add_argument(const std::string& argument);
        void add_arguments(const std::vector<std::string>& arguments);
        void add_credential(const std::optional<keyring::Credential>& credential);

    private:
        std::vector<std::string> m_arguments;
        ProcessOutputCallback m_output_callback;
        std::stop_token m_stop_token;
    };

    [[nodiscard]] std::string executable_path();
    [[nodiscard]] std::expected<std::string, std::string> execute(const Options& options);
}

#endif
