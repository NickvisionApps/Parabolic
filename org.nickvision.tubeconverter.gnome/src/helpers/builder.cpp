#include "helpers/builder.h"
#include <filesystem>
#include <stdexcept>
#include <libnick/app/aura.h>
#include <libnick/localization/gettext.h>
#include <libxml++/libxml++.h>

using namespace Nickvision::App;

namespace Nickvision::TubeConverter::GNOME
{
    GtkBuilder* BuilderHelpers::fromBlueprint(const std::string& blueprint)
    {
        std::filesystem::path path{ Aura::getActive().getExecutableDirectory() / "ui" / (blueprint + ".ui") };
        if(!std::filesystem::exists(path))
        {
            Aura::getActive().getLogger().log(Logging::LogLevel::Critical, "UI file not found. (" + path.string() + ")");
            throw std::runtime_error("UI file not found: " + path.string());
        }
        xmlpp::DomParser xml{ path.string() };
        xmlpp::Element* root{ xml.get_document()->get_root_node() };
        for(xmlpp::Node* node : root->find("//text()"))
        {
            xmlpp::Element* e{ node->get_parent() };
            if(e->get_attribute("translatable"))
            {
                xmlpp::TextNode* t{ dynamic_cast<xmlpp::TextNode*>(node) };
                e->remove_attribute("translatable");
                std::string context{ e->get_attribute_value("context") };
                if(!context.empty())
                {
                    std::string p{ context + "\004" +  t->get_content() };
                    t->set_content(Nickvision::Localization::Gettext::pgettext(p.c_str(), t->get_content().c_str()));
                }
                else
                {
                    t->set_content(_(t->get_content().c_str()));
                }
            }
        }
        Aura::getActive().getLogger().log(Logging::LogLevel::Debug, "Loaded builder from blueprint " + blueprint + ".");
        return gtk_builder_new_from_string(xml.get_document()->write_to_string().c_str(), -1);
    }
}
