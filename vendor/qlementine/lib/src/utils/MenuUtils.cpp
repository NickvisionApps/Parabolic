#include <oclero/qlementine/utils/MenuUtils.hpp>

#include <QMenu>
#include <QAction>
#include <QTimer>
#include <QPointer>

namespace oclero::qlementine {
class FlashActionHelper : QObject {
public:
  FlashActionHelper(QAction* action, QMenu* menu, const std::function<void()>& onAnimationFinished)
    : QObject(action)
    , _menu(menu)
    , _action(action)
    , _onAnimationFinished(onAnimationFinished) {
    if (_menu && _action) {
      _action->setProperty("qlementine_flashing", true);
      _menu->blockSignals(true);
      _timerId = startTimer(flashActionBlinkDuration);
    }
  }

  ~FlashActionHelper() override = default;

protected:
  void timerEvent(QTimerEvent*) override {
    if (_flashActionElapsedTime < flashActionDuration && _menu && _action) {
      _flashActionElapsedTime += flashActionBlinkDuration;
      const auto* currentActiveAction = _menu->activeAction();
      _menu->setActiveAction(currentActiveAction == nullptr ? _action : nullptr);
    } else {
      if (_timerId != -1) {
        killTimer(_timerId);
      }

      if (_menu) {
        if (_action) {
          _menu->setActiveAction(_action);
          _action->setProperty("qlementine_flashing", false);
        }
        _menu->blockSignals(false);
      }
      if (_onAnimationFinished) {
        _onAnimationFinished();
      }
      deleteLater();
    }
  }

private:
  static constexpr int flashActionBlinkDuration{ 60 }; // ms
  static constexpr int flashActionDuration{ flashActionBlinkDuration }; // ms
  int _flashActionElapsedTime{ 0 }; // ms
  int _timerId{ -1 };
  QPointer<QMenu> _menu{ nullptr };
  QPointer<QAction> _action{ nullptr };
  std::function<void()> _onAnimationFinished{};
};

QMenu* getTopLevelMenu(QMenu* menu) {
  auto parent = menu;
  while (parent != nullptr) {
    auto parent_menu = qobject_cast<QMenu*>(parent->parentWidget());
    if (parent_menu != nullptr)
      parent = parent_menu;
    else
      break;
  }
  return parent;
}

void flashAction(QAction* action, QMenu* menu, const std::function<void()>& onAnimationFinished) {
  new FlashActionHelper(action, menu, onAnimationFinished);
}
} // namespace oclero::qlementine
