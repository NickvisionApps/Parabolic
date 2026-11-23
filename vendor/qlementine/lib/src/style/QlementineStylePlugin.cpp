// SPDX-FileCopyrightText: UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: MIT

#include "QlementineStylePlugin.hpp"
#include "oclero/qlementine/style/QlementineStyle.hpp"

namespace oclero::qlementine {

QStyle* QlementineStylePlugin::create(const QString& key) {
  if (key.toLower() == QStringLiteral("qlementine")) {
    return new QlementineStyle();
  }
  return nullptr;
}

} // namespace oclero::qlementine
