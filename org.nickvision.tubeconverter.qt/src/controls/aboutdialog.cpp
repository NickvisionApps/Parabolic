#include "controls/aboutdialog.h"
#include "ui_aboutdialog.h"
#include <format>
#include <QClipboard>
#include <QPixmap>
#include <libnick/helpers/codehelpers.h>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>

using namespace Nickvision::App;
using namespace Nickvision::Helpers;
using namespace Nickvision::Update;

namespace Nickvision::TubeConverter::Qt::Controls
{
    static std::vector<std::string> keys(const std::unordered_map<std::string, std::string>& m)
    {
        std::vector<std::string> k;
        for(const std::pair<const std::string, std::string>& pair : m)
        {
            k.push_back(pair.first);
        }
        return k;
    }

    AboutDialog::AboutDialog(const AppInfo& appInfo, const std::string& debugInformation, QWidget* parent)
        : QDialog{ parent }, 
        m_ui{ new Ui::AboutDialog() }
    {
        m_ui->setupUi(this);
        setWindowTitle(_("About Parabolic"));
        //Load
        m_ui->lblIcon->setPixmap({ appInfo.getVersion().getVersionType() == VersionType::Stable ? ":/resources/org.nickvision.tubeconverter.svg" : ":/resources/org.nickvision.tubeconverter-devel.svg" });
        m_ui->lblAppName->setText(QString::fromStdString(appInfo.getShortName()));
        m_ui->lblAppDescription->setText(QString::fromStdString(appInfo.getDescription()));
        m_ui->btnAppVersion->setText(QString::fromStdString(appInfo.getVersion().str()));
        m_ui->btnAppVersion->setToolTip(QString::fromStdString(_("Copy Debug Information")));
        m_ui->lblChangelog->setText(_("Changelog:") + QString::fromStdString("\n" + appInfo.getChangelog()));
        m_ui->lblCredits->setText(QString::fromStdString(std::vformat(_("Developers:\n{}\nDesigners:\n{}\nArtists:\n{}"), std::make_format_args(CodeHelpers::unmove(StringHelpers::join(keys(appInfo.getDevelopers()), "\n", true)), CodeHelpers::unmove(StringHelpers::join(keys(appInfo.getDesigners()), "\n", true)), CodeHelpers::unmove(StringHelpers::join(keys(appInfo.getArtists()), "\n"))))));
        //Signals
        connect(m_ui->btnAppVersion, &QPushButton::clicked, [debugInformation](){ QApplication::clipboard()->setText(QString::fromStdString(debugInformation)); });
    }

    AboutDialog::~AboutDialog()
    {
        delete m_ui;
    }
}