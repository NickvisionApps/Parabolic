#ifndef PYTHONHELPERS_H
#define PYTHONHELPERS_H

#include <filesystem>
#include <libnick/logging/logger.h>
#include <pybind11/embed.h>

namespace Nickvision::TubeConverter::Shared::Helpers
{
    /**
     * @brief Helper functions for working with Python.
     */
    namespace PythonHelpers
    {
        /**
         * @brief Starts the Python interpreter and checks for required yt-dlp module.
         * @param logger The application logger
         * @return True if Python started, false otherwise
         */
        bool start(const Logging::Logger& logger);
        /**
         * @brief Shuts down the Python interpreter.
         * @param logger The application logger
         * @return True if Python was shut down, false otherwise
         */
        bool shutdown(const Logging::Logger& logger);
        /**
         * @brief Gets whether or not Python has been started.
         * @return True if Python has started, false otherwise
         */
        bool started();
        /**
         * @brief Sets the console output of the Python interpreter to a file.
         * @param path The path to the file to write the console output to
         * @return A Python object representing the file
         */
        pybind11::object setConsoleOutputFile(const std::filesystem::path& path);
        /**
         * @brief Gets the Python debug information.
         * @return The debug information
         */
        std::string getDebugInformation();
    }
}

#endif //PYTHONHELPERS_H