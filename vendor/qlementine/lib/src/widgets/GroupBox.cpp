// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/GroupBox.hpp>

#include <oclero/qlementine/Common.hpp>
#include <oclero/qlementine/widgets/Label.hpp>

#include <QEvent>
#include <QStyle>
#include <QStyleOptionGroupBox>

namespace oclero::qlementine {
GroupBox::GroupBox(QWidget* parent)
  : GroupBox(QString{}, parent) {}

GroupBox::GroupBox(const QString& title, QWidget* parent)
  : QGroupBox(title, parent)
  , _descriptionLabel(new Label(QString{}, TextRole::Caption, this)) {
  _descriptionLabel->setWordWrap(true);
  _descriptionLabel->hide();
}

QString GroupBox::description() const {
  return _descriptionLabel->text();
}

void GroupBox::setDescription(const QString& description) {
  if (description == _descriptionLabel->text()) {
    return;
  }

  _descriptionLabel->setText(description);
  _descriptionLabel->setVisible(!description.isEmpty());

  updateContentsMargins();
  updateDescriptionGeometry();
  updateGeometry();
  Q_EMIT descriptionChanged();
}

int GroupBox::descriptionHeight(int width) const {
  if (_descriptionLabel->text().isEmpty()) {
    return 0;
  }
  return _descriptionLabel->heightForWidth(width);
}

void GroupBox::changeEvent(QEvent* e) {
  QGroupBox::changeEvent(e);
  if (e->type() == QEvent::StyleChange || e->type() == QEvent::FontChange) {
    updateContentsMargins();
    updateDescriptionGeometry();
  }
}

void GroupBox::resizeEvent(QResizeEvent* e) {
  QGroupBox::resizeEvent(e);
  updateContentsMargins();
  updateDescriptionGeometry();
}

void GroupBox::showEvent(QShowEvent* e) {
  QGroupBox::showEvent(e);
  updateContentsMargins();
  updateDescriptionGeometry();
}

void GroupBox::updateContentsMargins() {
  // QGroupBox only computes its contents margins when its style, font or title change, but the description height
  // depends on the width: compute them like QGroupBox does, each time the width can have changed.
  QStyleOptionGroupBox option;
  initStyleOption(&option);
  const auto contentsRect = style()->subControlRect(QStyle::CC_GroupBox, &option, QStyle::SC_GroupBoxContents, this);
  setContentsMargins(contentsRect.left() - option.rect.left(), contentsRect.top() - option.rect.top(),
    option.rect.right() - contentsRect.right(), option.rect.bottom() - contentsRect.bottom());
}

void GroupBox::updateDescriptionGeometry() {
  QStyleOptionGroupBox option;
  initStyleOption(&option);
  const auto labelRect = style()->subControlRect(QStyle::CC_GroupBox, &option, QStyle::SC_GroupBoxLabel, this);
  const auto width = labelRect.width();
  _descriptionLabel->setGeometry(labelRect.x(), labelRect.y() + labelRect.height(), width, descriptionHeight(width));
}
} // namespace oclero::qlementine
