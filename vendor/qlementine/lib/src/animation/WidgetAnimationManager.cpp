// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/animation/WidgetAnimationManager.hpp>

namespace oclero::qlementine {
WidgetAnimationManager::WidgetAnimationManager(QObject* parent)
  : QObject(parent) {
  initializeEasingCurves();
}

bool WidgetAnimationManager::enabled() const {
  return _animationsEnabled;
}

void WidgetAnimationManager::setEnabled(bool enabled) {
  _animationsEnabled = enabled;
  if (!enabled) {
    stopAll();
  }
}

const WidgetAnimator* WidgetAnimationManager::getAnimator(const QWidget* w) const {
  const auto* res = findWidget(w);
  return res;
}

WidgetAnimator* WidgetAnimationManager::getOrCreateAnimator(const QWidget* w) {
  auto* animator = findWidget(w);
  if (!animator) {
    // The widget given by the QStyle is a const pointer, so we unfortunately need
    // this const_cast.
    auto* parentWidget = const_cast<QWidget*>(w);
    animator = new WidgetAnimator(parentWidget);
    addWidget(w, animator);
  }
  return animator;
}

void WidgetAnimationManager::addWidget(const QWidget* widget, WidgetAnimator* widgetAnimator) {
  if (!findWidget(widget)) {
    _map.insert_or_assign(widget, widgetAnimator);

    QObject::connect(widget, &QObject::destroyed, this, [this, widget]() {
      removeWidget(widget);
    });
  }
}

void WidgetAnimationManager::removeWidget(const QWidget* widget) {
  auto it = _map.find(widget);
  if (it != _map.end()) {
    auto* widgetAnimator = it->second;
    _map.erase(it);
    widgetAnimator->deleteLater();
  }
}

WidgetAnimator* WidgetAnimationManager::findWidget(const QWidget* widget) const {
  auto it = _map.find(widget);
  if (it != _map.end()) {
    return it->second;
  }
  return nullptr;
}

void WidgetAnimationManager::forEarch(const std::function<void(const QWidget* w, WidgetAnimator* a)>& cb) {
  if (!cb)
    return;
  std::for_each(_map.begin(), _map.end(), [&cb](const auto& kvp) {
    cb(kvp.first, kvp.second);
  });
}

void WidgetAnimationManager::stopAll() {
  forEarch([this](const QWidget* w, WidgetAnimator*) {
    // Stop all animations happening on the widget.
    removeWidget(w);
  });
}

void WidgetAnimationManager::initializeEasingCurves() {
  _focusEasingCurve.setOvershoot(5.);
  _focusEasingCurve.setType(QEasingCurve::Type::OutBack);
  _defaultEasingCurve.setType(QEasingCurve::Type::OutCubic);
  _linearEasingCurve.setType(QEasingCurve::Type::Linear);
}

const QEasingCurve& WidgetAnimationManager::focusEasingCurve() const {
  return _focusEasingCurve;
}

const QEasingCurve& WidgetAnimationManager::defaultEasingCurve() const {
  return _defaultEasingCurve;
}
} // namespace oclero::qlementine
