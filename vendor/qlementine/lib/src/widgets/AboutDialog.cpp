// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT
#include <oclero/qlementine/widgets/AboutDialog.hpp>

#include <QApplication>
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>
#include <QDesktopServices>

#include <oclero/qlementine/widgets/Label.hpp>

namespace oclero::qlementine {
AboutDialog::AboutDialog(QWidget* parent)
  : QDialog(parent) {
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setWindowModality(Qt::WindowModality::ApplicationModal);
  setWindowFlag(Qt::WindowType::MSWindowsFixedSizeDialogHint, true);
  setWindowFlag(Qt::WindowType::WindowContextHelpButtonHint, false);
  setWindowFlag(Qt::WindowType::WindowMaximizeButtonHint, false);
  setWindowFlag(Qt::WindowType::WindowMinimizeButtonHint, false);
  setWindowFlag(Qt::WindowType::WindowFullscreenButtonHint, false);

  setupUi();
}

void AboutDialog::setIcon(const QIcon& icon) {
  const auto iconSize = _iconLabel->height() * 2;
  const auto& newIcon = icon.isNull() ? QApplication::windowIcon() : icon;
  const auto pixmap = newIcon.pixmap(iconSize);
  _iconLabel->setPixmap(pixmap);
  _iconLabel->setVisible(!newIcon.isNull());
}

void AboutDialog::setApplicationName(const QString& name) {
  const auto trimmed = name.trimmed();
  _appNameLabel->setText(trimmed);
  _appNameLabel->setVisible(!_appNameLabel->text().isEmpty());
}

void AboutDialog::setApplicationVersion(const QString& version) {
  const auto trimmed = version.trimmed();
  _appVersionLabel->setText(trimmed);
  _appVersionLabel->setVisible(!_appVersionLabel->text().isEmpty());
}

void AboutDialog::setDescription(const QString& description) {
  const auto trimmed = description.trimmed();
  _appDescriptionLabel->setText(trimmed);
  _appDescriptionLabel->setVisible(!_appDescriptionLabel->text().isEmpty());
}

void AboutDialog::setWebsiteUrl(const QString& url) {
  auto prettyUrl = url.trimmed();
  if (prettyUrl.startsWith("https://")) {
    prettyUrl.remove(0, 8);
  }
  if (prettyUrl.isEmpty()) {
    _websiteLabel->setText("");
    _websiteLabel->setVisible(false);
  } else {
    _websiteLabel->setText(QString("<a href=\"%1\">%2</a>").arg(url, prettyUrl));
    _websiteLabel->setVisible(true);
  }
}

void AboutDialog::setLicense(const QString& license) {
  const auto trimmed = license.trimmed();
  _licenseLabel->setText(trimmed);
  _licenseLabel->setVisible(!_licenseLabel->text().isEmpty());
}

void AboutDialog::setCopyright(const QString& copyright) {
  _copyrightLabel->setText(copyright);
}

void AboutDialog::addSocialMediaLink(const QString& name, const QString& url, const QIcon& icon) {
  auto* button = new QPushButton(this);
  button->setIcon(icon);
  button->setFocusPolicy(Qt::NoFocus);
  button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  button->setToolTip(name);
  button->setFlat(true);
  button->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
  const auto button_url = QUrl(url);
  QObject::connect(button, &QPushButton::clicked, button, [button_url]() {
    QDesktopServices::openUrl(button_url);
  });
  _buttonsLayout->addWidget(button);
}

void AboutDialog::setupUi() {
  auto* rootLayout = new QVBoxLayout(this);
  rootLayout->setContentsMargins(32, 16, 32, 16);
  rootLayout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
  setLayout(rootLayout);

  // Logo, app name and version.
  {
    _iconLabel = new QLabel(this);
    _iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _iconLabel->setFixedSize(64, 64);
    _iconLabel->setScaledContents(true);
    _iconLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    const auto icon = QApplication::windowIcon();
    _iconLabel->setPixmap(icon.pixmap(_iconLabel->height() * 2));
    _iconLabel->setVisible(!icon.isNull());

    auto iconLabelLayout = new QHBoxLayout();
    iconLabelLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
    iconLabelLayout->addWidget(_iconLabel);
    iconLabelLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
    iconLabelLayout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);

    auto* appInfoLayout = new QVBoxLayout();
    appInfoLayout->setContentsMargins(0, 0, 0, 0);
    appInfoLayout->setSpacing(2);

    _appNameLabel = new oclero::qlementine::Label(this);
    _appNameLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    _appNameLabel->setRole(oclero::qlementine::TextRole::H4);
    _appNameLabel->setText(QApplication::applicationDisplayName());
    _appNameLabel->setAlignment(Qt::AlignCenter);

    _appVersionLabel = new QLabel(this);
    _appVersionLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    _appVersionLabel->setText(QApplication::applicationVersion());
    _appVersionLabel->setAlignment(Qt::AlignCenter);

    appInfoLayout->addWidget(_appNameLabel);
    appInfoLayout->addWidget(_appVersionLabel);

    rootLayout->addLayout(iconLabelLayout);
    rootLayout->addLayout(appInfoLayout);
  }

  // App description.
  {
    _appDescriptionLabel = new QLabel(this);
    _appDescriptionLabel->setAlignment(Qt::AlignCenter);
    _appDescriptionLabel->setWordWrap(true);
    _appDescriptionLabel->setFixedWidth(250);
    _appDescriptionLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    rootLayout->addWidget(_appDescriptionLabel);
    rootLayout->setAlignment(_appDescriptionLabel, Qt::AlignHCenter);
  }

  rootLayout->addSpacerItem(new QSpacerItem(0, 16, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));

  // Links to social media.
  {
    _buttonsLayout = new QHBoxLayout();
    _buttonsLayout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
    _buttonsLayout->setSpacing(4);
    _buttonsLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->addLayout(_buttonsLayout);
    rootLayout->setAlignment(_buttonsLayout, Qt::AlignHCenter);

    // Legal information.
    {
      _websiteLabel = new oclero::qlementine::Label(this);
      _websiteLabel->setAlignment(Qt::AlignCenter);
      _websiteLabel->setTextInteractionFlags(
        Qt::TextInteractionFlag::LinksAccessibleByMouse | Qt::TextInteractionFlag::LinksAccessibleByKeyboard);
      QObject::connect(_websiteLabel, &QLabel::linkActivated, this, [](const QString& link) {
        QDesktopServices::openUrl(QUrl(link));
      });

      _licenseLabel = new oclero::qlementine::Label(this);
      _licenseLabel->setRole(oclero::qlementine::TextRole::Caption);
      _licenseLabel->setText(QApplication::applicationDisplayName());
      _licenseLabel->setAlignment(Qt::AlignCenter);

      _copyrightLabel = new oclero::qlementine::Label(this);
      _copyrightLabel->setRole(oclero::qlementine::TextRole::Caption);
      _copyrightLabel->setAlignment(Qt::AlignCenter);

      auto* smallTextsLayout = new QVBoxLayout();
      smallTextsLayout->setContentsMargins(0, 0, 0, 0);
      smallTextsLayout->setSpacing(2);
      smallTextsLayout->addWidget(_licenseLabel);
      smallTextsLayout->setAlignment(_licenseLabel, Qt::AlignHCenter);

      smallTextsLayout->addWidget(_copyrightLabel);
      smallTextsLayout->setAlignment(_copyrightLabel, Qt::AlignHCenter);

      rootLayout->addWidget(_websiteLabel);
      rootLayout->setAlignment(_websiteLabel, Qt::AlignHCenter);

      rootLayout->addLayout(smallTextsLayout);
    }
  }
}
} // namespace oclero::qlementine
