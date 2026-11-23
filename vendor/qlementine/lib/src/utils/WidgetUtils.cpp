// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/utils/WidgetUtils.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>

#include <QFrame>
#include <QBoxLayout>
#include <QLayout>

namespace oclero::qlementine {
QWidget* makeHorizontalLine(QWidget* parentWidget, int maxWidth) {
  const auto* qlementineStyle =
    parentWidget ? qobject_cast<qlementine::QlementineStyle*>(parentWidget->style()) : nullptr;
  const auto lineThickness = qlementineStyle ? qlementineStyle->theme().borderWidth : 1;

  auto* line = new QFrame(parentWidget);
  line->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  line->setFixedHeight(lineThickness);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Plain);

  if (maxWidth >= 0) {
    line->setMaximumWidth(maxWidth);
  }

  return line;
}

QWidget* makeVerticalLine(QWidget* parentWidget, int maxHeight) {
  const auto* qlementineStyle =
    parentWidget ? qobject_cast<qlementine::QlementineStyle*>(parentWidget->style()) : nullptr;
  const auto lineThickness = qlementineStyle ? qlementineStyle->theme().borderWidth : 1;

  auto* line = new QFrame(parentWidget);
  line->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
  line->setFixedWidth(lineThickness);
  line->setFrameShape(QFrame::VLine);
  line->setFrameShadow(QFrame::Plain);

  if (maxHeight >= 0) {
    line->setMaximumHeight(maxHeight);
  }

  return line;
}

void centerWidget(QWidget* widget, QWidget* host) {
  if (!host)
    host = widget->parentWidget();

  if (host) {
    const auto& hostRect = host->geometry();
    widget->move(hostRect.center() - widget->rect().center());
  } else {
    const auto screenGeometry = QGuiApplication::screens().constFirst()->geometry();
    const auto x = (screenGeometry.width() - widget->width()) / 2;
    const auto y = (screenGeometry.height() - widget->height()) / 2;
    widget->move(x, y);
  }
}

qreal getDpi(const QWidget* widget) {
  if (widget) {
    if (const auto* screen = widget->screen()) {
      return screen->logicalDotsPerInch();
    }
  }
  return 72.;
}

QWindow* getWindow(const QWidget* widget) {
  if (widget) {
    if (auto* window = widget->window()) {
      return window->windowHandle();
    }
  }
  return nullptr;
}

void clearFocus(QWidget* widget, bool recursive) {
  if (widget) {
    widget->clearFocus();

    if (recursive) {
      const auto children = widget->findChildren<QWidget*>();
      for (auto* child : children) {
        clearFocus(child, recursive);
      }
    }
  }
}
} // namespace oclero::qlementine
