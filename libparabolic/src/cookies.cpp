#include "cookies.h"
#include <string>
#ifdef __linux__
#include <filesystem>
#include <sago/platform_folders.h>
#include <vector>
#include "environment.h"
#endif

namespace parabolic::cookies
{
    std::string to_argument_str(Browser browser)
    {
        std::string browser_str;
        switch (browser)
        {
        case Browser::Brave:
            browser_str = "brave";
            break;
        case Browser::Chrome:
            browser_str = "chrome";
            break;
        case Browser::Chromium:
            browser_str = "chromium";
            break;
        case Browser::Edge:
            browser_str = "edge";
            break;
        case Browser::Firefox:
            browser_str = "firefox";
            break;
        case Browser::Opera:
            browser_str = "opera";
            break;
        case Browser::Vivaldi:
            browser_str = "vivaldi";
            break;
        case Browser::Whale:
            browser_str = "whale";
            break;
        default:
            break;
        }
#ifdef __linux__
        if (environment::deployment_mode() == environment::DeploymentMode::Flatpak)
        {
            std::filesystem::path home{ sago::internal::getHome() };
            std::filesystem::path path;
            if (browser == Browser::Firefox)
            {
                std::vector<std::filesystem::path> candidates{
                    home / ".mozilla" / "firefox",
                    home / ".config" / "mozilla" / "firefox",
                    home / "snap" / "firefox" / "common" / ".mozilla" / "firefox",
                };
                path = candidates[0];
                for (const std::filesystem::path& candidate : candidates)
                {
                    if (std::filesystem::exists(candidate))
                    {
                        path = candidate;
                        break;
                    }
                }
            }
            else
            {
                switch (browser)
                {
                case Browser::Brave:
                    path = home / ".config" / "BraveSoftware" / "Brave-Browser";
                    break;
                case Browser::Chrome:
                    path = home / ".config" / "google-chrome";
                    break;
                case Browser::Chromium:
                    path = home / ".config" / "chromium";
                    break;
                case Browser::Edge:
                    path = home / ".config" / "microsoft-edge";
                    break;
                case Browser::Opera:
                    path = home / ".config" / "opera";
                    break;
                case Browser::Vivaldi:
                    path = home / ".config" / "vivaldi";
                    break;
                case Browser::Whale:
                    path = home / ".config" / "naver-whale";
                    break;
                default:
                    break;
                }
            }
            if (!path.empty())
            {
                browser_str = std::format("{}:{}", browser_str, path.string());
            }
        }
#endif
        return browser_str;
    }
}
