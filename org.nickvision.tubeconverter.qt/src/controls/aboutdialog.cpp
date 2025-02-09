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
        m_ui{ new Ui::AboutDialog() },
        m_debugInformation{ QString::fromStdString(debugInformation) }
    {
        m_ui->setupUi(this);
        setWindowTitle(_("About Parabolic"));
        //Load
        m_ui->tabWidget->setTabText(0, _("About"));
        m_ui->tabWidget->setTabText(1, _("Changelog"));
        m_ui->tabWidget->setTabText(2, _("Credits"));
        m_ui->tabWidget->setTabText(3, _("Debugging"));
        m_ui->tabWidget->setCurrentIndex(0);
        m_ui->lblAppIcon->setPixmap({ appInfo.getVersion().getVersionType() == VersionType::Stable ? ":/resources/org.nickvision.tubeconverter.svg" : ":/resources/org.nickvision.tubeconverter-devel.svg" });
        m_ui->lblAppName->setText(QString::fromStdString(appInfo.getShortName()));
        m_ui->lblAppDescription->setText(QString::fromStdString(appInfo.getDescription()));
        m_ui->lblAppVersion->setText(QString::fromStdString(appInfo.getVersion().str()));
        m_ui->lblChangelog->setText(QString::fromStdString(appInfo.getChangelog()));
        m_ui->groupDevelopers->setTitle(_("Developers"));
        m_ui->lblDevelopers->setText(QString::fromStdString(StringHelpers::join(keys(appInfo.getDevelopers()), "\n")));
        m_ui->groupDesigners->setTitle(_("Designers"));
        m_ui->lblDesigners->setText(QString::fromStdString(StringHelpers::join(keys(appInfo.getDesigners()), "\n")));
        m_ui->groupArtists->setTitle(_("Artists"));
        m_ui->lblArtists->setText(QString::fromStdString(StringHelpers::join(keys(appInfo.getArtists()), "\n")));
        m_ui->btnCopyDebugInformation->setText(QString::fromStdString(_("Copy Debug Information")));
        m_ui->lblDebug->setText(m_debugInformation);
        //Signals
        connect(m_ui->btnCopyDebugInformation, &QPushButton::clicked, this, &AboutDialog::copyDebugInformation);
    }

    AboutDialog::~AboutDialog()
    {
        delete m_ui;
    }

    void AboutDialog::copyDebugInformation()
    {
        QApplication::clipboard()->setText(m_debugInformation);
    }
}
