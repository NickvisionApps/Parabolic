#include "application.h"

using namespace Nickvision::TubeConverter::GNOME;

int main(int argc, char* argv[])
{
    Application app{ argc, argv };
    return app.run();
}