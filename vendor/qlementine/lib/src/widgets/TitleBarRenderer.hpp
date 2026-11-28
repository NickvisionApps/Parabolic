// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/widgets/TitleBar.hpp>

#include <memory>

class QPainter;

namespace oclero::qlementine {
class Theme;

struct CaptionButtonState {
  bool hovered{ false };
  bool pressed{ false };
  bool windowActive{ true };
  bool maximized{ false };
};

/// Draws a TitleBar and gives its metrics. One implementation per TitleBarStyle.
class TitleBarRenderer {
public:
  virtual ~TitleBarRenderer() = default;

  virtual int barHeight(const Theme& theme) const = 0;
  virtual QSize captionButtonSize(CaptionButton button, const Theme& theme) const = 0;
  virtual int captionButtonSpacing() const = 0;
  /// Outer margin between the bar edges and the first/last caption button.
  virtual QMargins captionButtonMargins() const = 0;

  virtual void paintBackground(QPainter& painter, const QRect& rect, const Theme& theme, const QColor& background,
    bool windowActive, bool maximized) const = 0;
  virtual void paintTitle(QPainter& painter, const QRect& rect, const QString& title, const Theme& theme,
    bool windowActive) const = 0;
  virtual void paintCaptionButton(
    QPainter& painter, const QRect& rect, CaptionButton button, const CaptionButtonState& state, const Theme& theme)
    const = 0;
};

/// Implemented in QlementineTitleBarRenderer.cpp.
std::unique_ptr<TitleBarRenderer> createQlementineTitleBarRenderer();

/// Implemented in AdwaitaTitleBarRenderer.cpp. Pure painting code, compiled on every platform.
std::unique_ptr<TitleBarRenderer> createAdwaitaTitleBarRenderer();
} // namespace oclero::qlementine
