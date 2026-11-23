// SPDX-FileCopyrightText: UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: MIT

#pragma once

#include <QStylePlugin>

namespace oclero::qlementine {

class QlementineStylePlugin : public QStylePlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface" FILE "qlementine_style_plugin.json")

public:
  QStyle* create(const QString& key) override;
};

} // namespace oclero::qlementine
