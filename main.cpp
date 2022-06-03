#include "ui/application.h"

int main(int argc, char* argv[])
{
    NickvisionTubeConverter::UI::Application app{"org.nickvision.tubeconverter"};
    return app.run(argc, argv);
}
