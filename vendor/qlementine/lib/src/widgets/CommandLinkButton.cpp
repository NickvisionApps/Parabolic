// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/CommandLinkButton.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/QlementineStyleOption.hpp>

#include <QStylePainter>

namespace oclero::qlementine {
CommandLinkButton::CommandLinkButton(QWidget* parent)
  : QCommandLinkButton(parent) {
  updateIconSize();
}

CommandLinkButton::CommandLinkButton(const QString& text, QWidget* parent)
  : QCommandLinkButton(text, parent) {
  updateIconSize();
}

CommandLinkButton::CommandLinkButton(const QString& text, const QString& description, QWidget* parent)
  : QCommandLinkButton(text, description, parent) {
  updateIconSize();
}

CommandLinkButton::CommandLinkButton(
  const QIcon& icon, const QString& text, const QString& description, QWidget* parent)
  : QCommandLinkButton(text, description, parent) {
  updateIconSize();
  setIcon(icon);
}

CommandLinkButton::~CommandLinkButton() = default;


QSize CommandLinkButton::sizeHint() const {
  if (const auto* qlementineStyle = qobject_cast<QlementineStyle*>(style())) {
    ensurePolished();
    QStyleOptionCommandLinkButton opt;
    initStyleOption(&opt);
    const auto minSize =
      qlementineStyle->sizeFromContentsExt(QlementineStyle::ContentsTypeExt::CT_CommandButton, &opt, { 0, 0 }, this);
    return minSize;
  } else {
    return QCommandLinkButton::sizeHint();
  }
}

int CommandLinkButton::heightForWidth(int w) const {
  return w;
}

bool CommandLinkButton::hasHeightForWidth() const {
  return false;
}

void CommandLinkButton::paintEvent(QPaintEvent* e) {
  if (const auto* qlementineStyle = qobject_cast<QlementineStyle*>(style())) {
    QPainter p(this);
    QStyleOptionCommandLinkButton opt;
    initStyleOption(&opt);
    qlementineStyle->drawControlExt(QlementineStyle::ControlElementExt::CE_CommandButton, &opt, &p, this);
  } else {
    QCommandLinkButton::paintEvent(e);
  }
}

void CommandLinkButton::initStyleOption(QStyleOptionCommandLinkButton* option) const {
  QCommandLinkButton::initStyleOption(option);
  option->description = description();
  option->iconSize = iconSize();
}

void CommandLinkButton::updateIconSize() {
  if (const auto* qlementineStyle = qobject_cast<QlementineStyle*>(style())) {
    const auto iconExtent =
      qlementineStyle->pixelMetricExt(QlementineStyle::PixelMetricExt::PM_MediumIconSize, nullptr, this);
    setIconSize(QSize(iconExtent, iconExtent));
  }
}

} // namespace oclero::qlementine
