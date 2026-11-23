// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/style/QlementineStyle.hpp>

#include <QItemDelegate>
#include <QPointer>

namespace oclero::qlementine {
class QLEMENTINE_EXPORT ComboBoxDelegate : public QItemDelegate {
public:
  ComboBoxDelegate(QWidget* widget, QlementineStyle& style);
  ~ComboBoxDelegate() override = default;

protected:
  void paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& idx) const override;
  QSize sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& idx) const override;

private:
  const QWidget* _widget{ nullptr };
  const QPointer<QlementineStyle> _qlementineStyle;
};
} // namespace oclero::qlementine
