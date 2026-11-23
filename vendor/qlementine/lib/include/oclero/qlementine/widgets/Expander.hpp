// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QWidget>
#include <QPointer>
#include <QVariantAnimation>

namespace oclero::qlementine {
/// A QWidget that allows to expand vertically or horizontally,
/// revealing or hiding its content with an animation.
class QLEMENTINE_EXPORT Expander : public QWidget {
  Q_OBJECT

  Q_PROPERTY(bool expanded READ expanded WRITE setExpanded NOTIFY expandedChanged)
  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)

public:
  explicit Expander(QWidget* parent = nullptr);
  ~Expander() override = default;

  bool expanded() const;
  Q_SLOT void setExpanded(bool expanded);
  void toggleExpanded();

  Qt::Orientation orientation() const;
  Q_SLOT void setOrientation(Qt::Orientation orientation);

  QWidget* content() const;
  void setContent(QWidget* content);

  QSize sizeHint() const override;

Q_SIGNALS:
  void expandedChanged();
  void aboutToExpand();
  void aboutToShrink();
  void didExpand();
  void didShrink();
  void orientationChanged();
  void contentChanged();

protected:
  bool event(QEvent* e) override;
  void resizeEvent(QResizeEvent* e) override;
  bool eventFilter(QObject* o, QEvent* e) override;

private:
  void updateContentGeometry();

private:
  bool _expanded{ false };
  Qt::Orientation _orientation{ Qt::Orientation::Vertical };
  QVariantAnimation _animation;
  QPointer<QWidget> _content{ nullptr };
};
} // namespace oclero::qlementine
