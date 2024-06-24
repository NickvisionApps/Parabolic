#include "helpers/pythonhelpers.h"

namespace py = pybind11;

namespace Nickvision::TubeConverter::Helpers
{
    py::object PythonHelpers::setConsoleOutputFile(const std::filesystem::path& path)
    {
        py::module_ sys{ py::module_::import("sys") };
        py::module_ io{ py::module_::import("io") };
        py::object file{ io.attr("open")(path.string(), "w") };
        sys.attr("stdout") = file;
        sys.attr("stderr") = file;
        return file;
    }
}