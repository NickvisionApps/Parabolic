#include "pages/home_page.h"
#include <QApplication>
#include <QCommandLinkButton>
#include <QDesktopServices>
#include <QIcon>
#include <QObject>
#include <QPalette>
#include <QPushButton>
#include <QSize>
#include <QString>
#include <QUrl>
#include <QWidget>
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include "app.h"
#include "application.h"
#include "helpers/translation_notifier.h"
#include "translation.h"
#include "ui_home_page.h"

namespace parabolic::qt::pages
{
    HomePage::HomePage(QWidget* parent)
        : QWidget{ parent },
          m_ui{ new Ui::HomePage() }
    {
        m_ui->setupUi(this);
        setBackgroundRole(QPalette::Base);
        m_ui->lbl_icon->setPixmap(QIcon(":/org.nickvision.tubeconverter.svg").pixmap(QSize(96, 96), devicePixelRatioF()));
        m_ui->btn_add_download->setIcon(
                oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                                                       oclero::qlementine::icons::Icons16::Document_New),
                                                               oclero::qlementine::appStyle()->theme().iconSizeMedium));
        m_ui->btn_tag_music_library->setIcon(
                oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                                                       oclero::qlementine::icons::Icons16::Misc_Library),
                                                               oclero::qlementine::appStyle()->theme().iconSizeMedium));
        m_ui->btn_whats_new->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Misc_Stars)));
        m_ui->btn_github->setIcon(
                oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                                                       oclero::qlementine::icons::Icons16::Brand_Github),
                                                               oclero::qlementine::appStyle()->theme().iconSizeMedium));
        m_ui->btn_report_an_issue->setIcon(
                oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                                                       oclero::qlementine::icons::Icons16::Misc_Debug),
                                                               oclero::qlementine::appStyle()->theme().iconSizeMedium));
        m_ui->btn_discussions->setIcon(
                oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                                                       oclero::qlementine::icons::Icons16::Misc_Comment),
                                                               oclero::qlementine::appStyle()->theme().iconSizeMedium));
        m_ui->btn_keyring->setIcon(
                oclero::qlementine::appStyle()->makeThemedIcon(oclero::qlementine::icons::iconPath(
                                                                       oclero::qlementine::icons::Icons16::Misc_Password),
                                                               oclero::qlementine::appStyle()->theme().iconSizeMedium));
        m_ui->btn_whats_new->setBadgeVisible(true);
        translate();
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &HomePage::translate);
        connect(m_ui->btn_add_download, &QCommandLinkButton::clicked, this, &HomePage::add_download_requested);
        connect(m_ui->btn_tag_music_library, &QCommandLinkButton::clicked, this, &HomePage::tag_music_library_requested);
        connect(m_ui->btn_keyring, &QCommandLinkButton::clicked, this, &HomePage::keyring_requested);
        connect(m_ui->btn_whats_new, &QPushButton::clicked, this, &HomePage::whats_new);
        connect(m_ui->btn_github, &QCommandLinkButton::clicked, this, &HomePage::github_repository);
        connect(m_ui->btn_report_an_issue, &QCommandLinkButton::clicked, this, &HomePage::report_an_issue);
        connect(m_ui->btn_discussions, &QCommandLinkButton::clicked, this, &HomePage::discussions);
    }

    HomePage::~HomePage()
    {
        delete m_ui;
    }

    void HomePage::discussions()
    {
        QDesktopServices::openUrl(QUrl(QString::fromStdString(application::State::instance().product().discussions_url())));
    }

    void HomePage::github_repository()
    {
        QDesktopServices::openUrl(QUrl(QString::fromStdString(application::State::instance().product().repo_url())));
    }

    void HomePage::report_an_issue()
    {
        QDesktopServices::openUrl(QUrl(QString::fromStdString(application::State::instance().product().issues_url())));
    }

    void HomePage::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        const application::Product& product{ application::State::instance().product() };
        m_ui->lbl_name->setText(QString::fromStdString(product.short_name()));
        m_ui->lbl_version->setText(QString::fromStdString(translator._("Version {}", product.version().to_string())));
        m_ui->lbl_description->setText(QString::fromStdString(product.description()));
        m_ui->lbl_start->setText(translator._("Start"));
        m_ui->btn_add_download->setText(translator._("Add Download"));
        m_ui->btn_add_download->setDescription(translator._("Create a new video or audio download."));
        m_ui->btn_tag_music_library->setText(translator._("Tag Music Library"));
        m_ui->btn_tag_music_library->setDescription(translator._("Open a music library for tagging."));
        m_ui->btn_keyring->setText(translator._("Keyring"));
        m_ui->btn_keyring->setDescription(translator._("Manage credentials for your websites."));
        m_ui->btn_whats_new->setText(translator._("What's New?"));
        m_ui->lbl_help->setText(translator._("Help"));
        m_ui->btn_github->setText(translator._("GitHub Repository"));
        m_ui->btn_github->setDescription(translator._("Browse the source code."));
        m_ui->btn_report_an_issue->setText(translator._("Report an Issue"));
        m_ui->btn_report_an_issue->setDescription(translator._("File a bug or feature request."));
        m_ui->btn_discussions->setText(translator._("Discussions"));
        m_ui->btn_discussions->setDescription(translator._("Chat with the developer community."));
    }

    void HomePage::whats_new()
    {
        m_ui->btn_whats_new->setBadgeVisible(false);
        whats_new_requested();
    }
}
