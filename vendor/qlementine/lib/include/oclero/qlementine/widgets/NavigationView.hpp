// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/widgets/NavigationSidebar.hpp>

#include <QWidget>
#include <QStackedWidget>

namespace oclero::qlementine {
/// A NavigationSidebar and a QStackedWidget, where selecting an item shows its page.
/// The view takes ownership of the pages.
/// Use sidebar() to add sections and separators, set the header and footer, or toggle the compact mode.
/// The sidebar can be moved out of the view, for instance with UnifiedWindow::setLeadingWidget(), to extend it up to
/// the top of the window.
/// A separator is drawn between the sidebar and the pages, unless the view is in a UnifiedWindow, or the sidebar was
/// moved out of the view.
class QLEMENTINE_EXPORT NavigationView : public QWidget {
  Q_OBJECT

  Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
  Q_PROPERTY(int autoCompactWidth READ autoCompactWidth WRITE setAutoCompactWidth NOTIFY autoCompactWidthChanged)

public:
  explicit NavigationView(QWidget* parent = nullptr);
  ~NavigationView() override = default;

  NavigationSidebar* sidebar() const;
  QStackedWidget* stackedWidget() const;

  int addPage(QWidget* page, const QString& text, const QIcon& icon = {}, const QString& badge = {});
  int addBottomPage(QWidget* page, const QString& text, const QIcon& icon = {}, const QString& badge = {});
  int addChildPage(
    int parentIndex, QWidget* page, const QString& text, const QIcon& icon = {}, const QString& badge = {});
  void removePage(int index);

  QWidget* page(int index) const;
  QWidget* currentPage() const;

  int currentIndex() const;
  Q_SLOT void setCurrentIndex(int index);

  /// The sidebar becomes compact when the view is narrower than this width. A value of 0 disables it.
  int autoCompactWidth() const;
  Q_SLOT void setAutoCompactWidth(int width);

Q_SIGNALS:
  void currentIndexChanged();
  void autoCompactWidthChanged();

protected:
  void resizeEvent(QResizeEvent* e) override;
  void showEvent(QShowEvent* e) override;
  bool eventFilter(QObject* watched, QEvent* e) override;

private:
  QVariant makePageData(QWidget* page);
  int availableWidth() const;
  void updateSeparatorVisibility();

private:
  NavigationSidebar* _sidebar{ nullptr };
  QWidget* _separator{ nullptr };
  QStackedWidget* _stackedWidget{ nullptr };
  int _autoCompactWidth{ 0 };
};
} // namespace oclero::qlementine
