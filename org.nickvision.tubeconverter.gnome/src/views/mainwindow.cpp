#include "views/mainwindow.h"
#include <filesystem>
#include <format>
#include <libnick/app/appinfo.h>
#include <libnick/helpers/codehelpers.h>
#include <libnick/notifications/shellnotification.h>
#include <libnick/localization/gettext.h>
#include "helpers/builder.h"
#include "helpers/dialogptr.h"
#include "views/preferencesdialog.h"

using namespace Nickvision::App;
using namespace Nickvision::Events;
using namespace Nickvision::Helpers;
using namespace Nickvision::Notifications;
using namespace Nickvision::TubeConverter::GNOME::Helpers;
using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::Update;

namespace Nickvision::TubeConverter::GNOME::Views
{
    MainWindow::MainWindow(const std::shared_ptr<MainWindowController>& controller, GtkApplication* app)
        : m_controller{ controller },
        m_app{ app },
        m_builder{ BuilderHelpers::fromBlueprint("main_window") },
        m_window{ ADW_APPLICATION_WINDOW(gtk_builder_get_object(m_builder, "root")) }
    {
        //Setup Window
        gtk_application_add_window(GTK_APPLICATION(app), GTK_WINDOW(m_window));
        gtk_window_set_title(GTK_WINDOW(m_window), m_controller->getAppInfo().getShortName().c_str());
        gtk_window_set_icon_name(GTK_WINDOW(m_window), m_controller->getAppInfo().getId().c_str());
        if(m_controller->getAppInfo().getVersion().getVersionType() == VersionType::Preview)
        {
            gtk_widget_add_css_class(GTK_WIDGET(m_window), "devel");
        }
        adw_window_title_set_title(ADW_WINDOW_TITLE(gtk_builder_get_object(m_builder, "title")), m_controller->getAppInfo().getShortName().c_str());
        //Register Events
        g_signal_connect(m_window, "close_request", G_CALLBACK(+[](GtkWindow*, gpointer data) -> bool { return reinterpret_cast<MainWindow*>(data)->onCloseRequested(); }), this);
        m_controller->notificationSent() += [&](const NotificationSentEventArgs& args) { onNotificationSent(args); };
        m_controller->shellNotificationSent() += [&](const ShellNotificationSentEventArgs& args) { onShellNotificationSent(args); };
        m_controller->disclaimerTriggered() += [&](const ParamEventArgs<std::string>& args) { onDisclaimerTriggered(args); };
        //Quit Action
        GSimpleAction* actQuit{ g_simple_action_new("quit", nullptr) };
        g_signal_connect(actQuit, "activate", G_CALLBACK(+[](GSimpleAction*, GVariant*, gpointer data){ reinterpret_cast<MainWindow*>(data)->quit(); }), this);
        g_action_map_add_action(G_ACTION_MAP(m_window), G_ACTION(actQuit));
        SET_ACCEL_FOR_ACTION(m_app, "win.quit", "<Ctrl>Q");
        //Preferences Action
        GSimpleAction* actPreferences{ g_simple_action_new("preferences", nullptr) };
        g_signal_connect(actPreferences, "activate", G_CALLBACK(+[](GSimpleAction*, GVariant*, gpointer data){ reinterpret_cast<MainWindow*>(data)->preferences(); }), this);
        g_action_map_add_action(G_ACTION_MAP(m_window), G_ACTION(actPreferences));
        SET_ACCEL_FOR_ACTION(m_app, "win.preferences", "<Ctrl>comma");
        //Keyboard Shortcuts Action
        GSimpleAction* actKeyboardShortcuts{ g_simple_action_new("keyboardShortcuts", nullptr) };
        g_signal_connect(actKeyboardShortcuts, "activate", G_CALLBACK(+[](GSimpleAction*, GVariant*, gpointer data){ reinterpret_cast<MainWindow*>(data)->keyboardShortcuts(); }), this);
        g_action_map_add_action(G_ACTION_MAP(m_window), G_ACTION(actKeyboardShortcuts));
        SET_ACCEL_FOR_ACTION(m_app, "win.keyboardShortcuts", "<Ctrl>question");
        //About Action
        GSimpleAction* actAbout{ g_simple_action_new("about", nullptr) };
        g_signal_connect(actAbout, "activate", G_CALLBACK(+[](GSimpleAction*, GVariant*, gpointer data){ reinterpret_cast<MainWindow*>(data)->about(); }), this);
        g_action_map_add_action(G_ACTION_MAP(m_window), G_ACTION(actAbout));
        SET_ACCEL_FOR_ACTION(m_app, "win.about", "F1");
    }

    MainWindow::~MainWindow()
    {
        gtk_window_destroy(GTK_WINDOW(m_window));
        g_object_unref(m_builder);
    }

    GObject* MainWindow::gobj() const
    {
        return G_OBJECT(m_window);
    }

    void MainWindow::show()
    {
        WindowGeometry geometry{ m_controller->startup(m_controller->getAppInfo().getId() + ".desktop") };
        gtk_window_set_default_size(GTK_WINDOW(m_window), static_cast<int>(geometry.getWidth()), static_cast<int>(geometry.getHeight()));
        if(geometry.isMaximized())
        {
            gtk_window_maximize(GTK_WINDOW(m_window));
        }
        gtk_window_present(GTK_WINDOW(m_window));
    }

    bool MainWindow::onCloseRequested()
    {
        if(!m_controller->canShutdown())
        {
            return true;
        }
        int width;
        int height;
        gtk_window_get_default_size(GTK_WINDOW(m_window), &width, &height);
        m_controller->shutdown({ width, height, static_cast<bool>(gtk_window_is_maximized(GTK_WINDOW(m_window))) });
        return false;
    }

    void MainWindow::onNotificationSent(const NotificationSentEventArgs& args)
    {
        AdwToast* toast{ adw_toast_new(args.getMessage().c_str()) };
        adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(gtk_builder_get_object(m_builder, "toastOverlay")), toast);
    }

    void MainWindow::onShellNotificationSent(const ShellNotificationSentEventArgs& args)
    {
        m_controller->log(Logging::LogLevel::Debug, "ShellNotification sent. (" + args.getMessage() + ")");
        ShellNotification::send(args, m_controller->getAppInfo().getId(), _("Open"));
    }

    void MainWindow::onDisclaimerTriggered(const ParamEventArgs<std::string>& args)
    {
        AdwAlertDialog* dialog{ ADW_ALERT_DIALOG(adw_alert_dialog_new(_("Disclaimer"), args.getParam().c_str())) };
        adw_alert_dialog_set_extra_child(dialog, gtk_check_button_new_with_label(_("Don't show this message again")));
        adw_alert_dialog_add_responses(dialog, "close", _("Close"), nullptr);
        adw_alert_dialog_set_default_response(dialog, "close");
        adw_alert_dialog_set_close_response(dialog, "close");
        g_signal_connect(dialog, "response", G_CALLBACK(+[](AdwAlertDialog* self, const char*, gpointer data)
        {
            MainWindow* mainWindow{ reinterpret_cast<MainWindow*>(data) };
            mainWindow->m_controller->setShowDisclaimerOnStartup(!gtk_check_button_get_active(GTK_CHECK_BUTTON(adw_alert_dialog_get_extra_child(self))));
        }), this);
        adw_dialog_present(ADW_DIALOG(dialog), GTK_WIDGET(m_window));
    }

    void MainWindow::quit()
    {
        if(!onCloseRequested())
        {
            g_application_quit(G_APPLICATION(m_app));
        }
    }

    void MainWindow::preferences()
    {
        DialogPtr<PreferencesDialog> dialog{ m_controller->createPreferencesViewController(), GTK_WINDOW(m_window) };
        dialog->present();
    }

    void MainWindow::keyboardShortcuts()
    {
        GtkBuilder* builderHelp{ BuilderHelpers::fromBlueprint("shortcuts_dialog") };
        GtkShortcutsWindow* shortcuts{ GTK_SHORTCUTS_WINDOW(gtk_builder_get_object(builderHelp, "root")) };
        gtk_window_set_transient_for(GTK_WINDOW(shortcuts), GTK_WINDOW(m_window));
        gtk_window_set_icon_name(GTK_WINDOW(shortcuts), m_controller->getAppInfo().getId().c_str());
        g_signal_connect(shortcuts, "close-request", G_CALLBACK(+[](GtkWindow*, gpointer data){ g_object_unref(reinterpret_cast<GtkBuilder*>(data)); }), builderHelp);
        gtk_window_present(GTK_WINDOW(shortcuts));
    }

    void MainWindow::about()
    {
        std::string extraDebug;
        extraDebug += "GTK " + std::to_string(gtk_get_major_version()) + "." + std::to_string(gtk_get_minor_version()) + "." + std::to_string(gtk_get_micro_version()) + "\n";
        extraDebug += "libadwaita " + std::to_string(adw_get_major_version()) + "." + std::to_string(adw_get_minor_version()) + "." + std::to_string(adw_get_micro_version());
        AdwAboutDialog* dialog{ ADW_ABOUT_DIALOG(adw_about_dialog_new()) };
        adw_about_dialog_set_application_name(dialog, m_controller->getAppInfo().getShortName().c_str());
        adw_about_dialog_set_application_icon(dialog, std::string(m_controller->getAppInfo().getId() + (m_controller->getAppInfo().getVersion().getVersionType() == VersionType::Preview  ? "-devel" : "")).c_str());
        adw_about_dialog_set_developer_name(dialog, "Nickvision");
        adw_about_dialog_set_version(dialog, m_controller->getAppInfo().getVersion().str().c_str());
        adw_about_dialog_set_release_notes(dialog, m_controller->getAppInfo().getHtmlChangelog().c_str());
        adw_about_dialog_set_debug_info(dialog, m_controller->getDebugInformation(extraDebug).c_str());
        adw_about_dialog_set_comments(dialog, m_controller->getAppInfo().getDescription().c_str());
        adw_about_dialog_set_license_type(dialog, GTK_LICENSE_GPL_3_0);
        adw_about_dialog_set_copyright(dialog, "© Nickvision 2021-2024");
        adw_about_dialog_set_website(dialog, "https://nickvision.org/");
        adw_about_dialog_set_issue_url(dialog, m_controller->getAppInfo().getIssueTracker().c_str());
        adw_about_dialog_set_support_url(dialog, m_controller->getAppInfo().getSupportUrl().c_str());
        adw_about_dialog_add_link(dialog, _("GitHub Repo"), m_controller->getAppInfo().getSourceRepo().c_str());
        for(const std::pair<std::string, std::string>& pair : m_controller->getAppInfo().getExtraLinks())
        {
            adw_about_dialog_add_link(dialog, pair.first.c_str(), pair.second.c_str());
        }
        std::vector<const char*> urls;
        std::vector<std::string> developers{ AppInfo::convertUrlMapToVector(m_controller->getAppInfo().getDevelopers()) };
        for(const std::string& developer : developers)
        {
            urls.push_back(developer.c_str());
        }
        urls.push_back(nullptr);
        adw_about_dialog_set_developers(dialog, &urls[0]);
        urls.clear();
        std::vector<std::string> designers{ AppInfo::convertUrlMapToVector(m_controller->getAppInfo().getDesigners()) };
        for(const std::string& designer : designers)
        {
            urls.push_back(designer.c_str());
        }
        urls.push_back(nullptr);
        adw_about_dialog_set_designers(dialog, &urls[0]);
        urls.clear();
        std::vector<std::string> artists{ AppInfo::convertUrlMapToVector(m_controller->getAppInfo().getArtists()) };
        for(const std::string& artist : artists)
        {
            urls.push_back(artist.c_str());
        }
        urls.push_back(nullptr);
        adw_about_dialog_set_artists(dialog, &urls[0]);
        adw_about_dialog_set_translator_credits(dialog, m_controller->getAppInfo().getTranslatorCredits().c_str());
        adw_dialog_present(ADW_DIALOG(dialog), GTK_WIDGET(m_window));
    }
}
