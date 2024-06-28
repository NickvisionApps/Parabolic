#ifndef PYTHONHELPERS_H
#define PYTHONHELPERS_H

#include <filesystem>
#include <pybind11/embed.h>

namespace Nickvision::TubeConverter::Helpers
{
    /**
     * @brief Helper functions for working with Python.
     */
    namespace PythonHelpers
    {
        /**
         * @brief Sets the console output of the Python interpreter to a file.
         * @param path The path to the file to write the console output to
         * @return A Python object representing the file
         */
        pybind11::object setConsoleOutputFile(const std::filesystem::path& path);
    }
}

#endif //PYTHONHELPERS_H