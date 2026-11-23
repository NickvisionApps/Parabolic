// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT
#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QDialog>

class QLabel;
class QHBoxLayout;

namespace oclero::qlementine {
class Label;

/// @brief A dialog that displays information about the application.
/// It includes the application icon, description, website link, license information,
/// copyright notice, and social media links.
/// It will automatically use the application's icon and name if not set explicitly.
class QLEMENTINE_EXPORT AboutDialog : public QDialog {
  Q_OBJECT

public:
  explicit AboutDialog(QWidget* parent = nullptr);
  ~AboutDialog() override = default;

  void setIcon(const QIcon& icon);
  void setApplicationName(const QString& name);
  void setApplicationVersion(const QString& version);
  void setDescription(const QString& description);
  void setWebsiteUrl(const QString& url);
  void setLicense(const QString& license);
  void setCopyright(const QString& copyright);
  void addSocialMediaLink(const QString& name, const QString& url, const QIcon& icon = QIcon());

private:
  void setupUi();

private:
  QLabel* _iconLabel{ nullptr };
  Label* _appNameLabel{ nullptr };
  QLabel* _appVersionLabel{ nullptr };
  QLabel* _appDescriptionLabel{ nullptr };
  QLabel* _websiteLabel{ nullptr };
  Label* _licenseLabel{ nullptr };
  Label* _copyrightLabel{ nullptr };
  QHBoxLayout* _buttonsLayout{ nullptr };
};
} // namespace oclero::qlementine
