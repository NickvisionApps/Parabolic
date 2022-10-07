#include "ui/application.hpp"

using namespace NickvisionTubeConverter::UI;

/**
 * The main functions
 *
 * @param The number of arguments
 * @param The array of arguments
 *
 * @returns The application exit code
 */
int main(int argc, char* argv[])
{
    Application app("org.nickvision.tubeconverter");
    return app.run(argc, argv);
}

