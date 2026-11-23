// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/tools/ThemeEditor.hpp>

#include <oclero/qlementine/widgets/ColorEditor.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include <oclero/qlementine/widgets/LineEdit.hpp>

#include <QPushButton>
#include <QBoxLayout>
#include <QFormLayout>
#include <QStyle>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QStandardPaths>

#define ADD_TITLE(TEXT) \
  formLayout->addItem(new QSpacerItem(0, vSpacing * 3, QSizePolicy::Ignored, QSizePolicy::Fixed)); \
  formLayout->addRow(new Label(TEXT, TextRole::H3, &owner));

#define ADD_SUBTITLE(TEXT) \
  formLayout->addItem(new QSpacerItem(0, vSpacing / 2, QSizePolicy::Ignored, QSizePolicy::Fixed)); \
  formLayout->addRow(new Label(TEXT, TextRole::H4, &owner));

#define ADD_DESCRIPTION(TEXT) formLayout->addRow(new Label(TEXT, TextRole::Caption, &owner));

#define ADD_COLOR_EDITOR(NAME, DESCRIPTION) \
  { \
    const auto pair = makeColorEditorAndLabel(#NAME, DESCRIPTION, &owner, theme.NAME, [this](const QColor& c) { \
      theme.NAME = c; \
      Q_EMIT owner.themeChanged(theme); \
    }); \
    this->NAME##Editor = pair.second; \
    formLayout->addRow(pair.first, pair.second); \
  }

#define ADD_METADATA_TEXT_EDITOR(NAME, DESCRIPTION) \
  { \
    const auto pair = makeTextEditorAndLabel(#NAME, DESCRIPTION, &owner, [this](const QString& s) { \
      theme.meta.NAME = s; \
      Q_EMIT owner.themeChanged(theme); \
    }); \
    this->NAME##Editor = pair.second; \
    formLayout->addRow(pair.first, pair.second); \
  }

#define ADD_INT_EDITOR(NAME, DESCRIPTION) \
  { \
    const auto pair = makeTextEditorAndLabel(#NAME, DESCRIPTION, &owner, [this](const QString& s) { \
      auto ok = false; \
      const auto value = s.toInt(&ok); \
      if (ok && value > 0) { \
        theme.NAME = value; \
        Q_EMIT owner.themeChanged(theme); \
      } \
    }); \
    pair.second->setText(QString::number(theme.NAME)); \
    this->NAME##Editor = pair.second; \
    formLayout->addRow(pair.first, pair.second); \
  }

#define UPDATE_COLOR_EDITOR(NAME) \
  this->NAME##Editor->blockSignals(true); \
  this->NAME##Editor->setColor(theme.NAME); \
  this->NAME##Editor->blockSignals(false);

#define UPDATE_INT_EDITOR(NAME) \
  this->NAME##Editor->blockSignals(true); \
  this->NAME##Editor->setText(QString::number(theme.NAME)); \
  this->NAME##Editor->blockSignals(false);

#define UPDATE_METADATA_TEXT_EDITOR(NAME) \
  this->NAME##Editor->blockSignals(true); \
  this->NAME##Editor->setText(theme.meta.NAME); \
  this->NAME##Editor->blockSignals(false);

namespace oclero::qlementine {
const QString PREVIOUS_PATH_SETTINGS_KEY{ "previousPath" };
const QString DEFAULT_FILE_NAME{ "theme.json" };

std::pair<QWidget*, ColorEditor*> makeColorEditorAndLabel(const QString& label, const QString& description,
  QWidget* parent, const QColor& initialValue, const std::function<void(const QColor&)>& onChanged) {
  auto* colorEditor = new ColorEditor(initialValue, parent);
  QObject::connect(colorEditor, &ColorEditor::colorChanged, parent, [colorEditor, onChanged]() {
    onChanged(colorEditor->color());
  });

  auto* leftColumn = new QWidget(parent);
  auto* leftColumnLayout = new QVBoxLayout(leftColumn);
  leftColumnLayout->setContentsMargins(0, 0, 0, 0);
  const auto vSpacing = leftColumn->style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
  leftColumnLayout->setSpacing(vSpacing / 4);
  auto* nameLabel = new Label(label, TextRole::Default, leftColumn);
  nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  leftColumnLayout->addWidget(nameLabel);
  if (!description.isEmpty()) {
    auto* descriptionLabel = new Label(description, TextRole::Caption, leftColumn);
    leftColumnLayout->addWidget(descriptionLabel);
  }

  return { leftColumn, colorEditor };
}

std::pair<QWidget*, LineEdit*> makeTextEditorAndLabel(const QString& label, const QString& description, QWidget* parent,
  const std::function<void(const QString&)>& onChanged) {
  auto* lineEdit = new LineEdit(parent);
  lineEdit->setPlaceholderText(label);
  QObject::connect(lineEdit, &QLineEdit::editingFinished, parent, [lineEdit, onChanged] {
    onChanged(lineEdit->text().trimmed());
  });

  auto* leftColumn = new QWidget(parent);
  auto* leftColumnLayout = new QVBoxLayout(leftColumn);
  leftColumnLayout->setContentsMargins(0, 0, 0, 0);
  const auto vSpacing = leftColumn->style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
  leftColumnLayout->setSpacing(vSpacing / 4);
  auto* nameLabel = new Label(label, TextRole::Default, leftColumn);
  nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  leftColumnLayout->addWidget(nameLabel);
  if (!description.isEmpty()) {
    auto* descriptionLabel = new Label(description, TextRole::Caption, leftColumn);
    leftColumnLayout->addWidget(descriptionLabel);
  }

  return { leftColumn, lineEdit };
}

struct ThemeEditor::Impl {
  ColorEditor* primaryColorEditor{};
  ColorEditor* primaryColorHoveredEditor{};
  ColorEditor* primaryColorPressedEditor{};
  ColorEditor* primaryColorDisabledEditor{};

  ColorEditor* primaryColorForegroundEditor{};
  ColorEditor* primaryColorForegroundHoveredEditor{};
  ColorEditor* primaryColorForegroundPressedEditor{};
  ColorEditor* primaryColorForegroundDisabledEditor{};

  ColorEditor* primaryAlternativeColorEditor{};
  ColorEditor* primaryAlternativeColorHoveredEditor{};
  ColorEditor* primaryAlternativeColorPressedEditor{};
  ColorEditor* primaryAlternativeColorDisabledEditor{};

  ColorEditor* secondaryColorEditor{};
  ColorEditor* secondaryColorHoveredEditor{};
  ColorEditor* secondaryColorPressedEditor{};
  ColorEditor* secondaryColorDisabledEditor{};

  ColorEditor* secondaryColorForegroundEditor{};
  ColorEditor* secondaryColorForegroundHoveredEditor{};
  ColorEditor* secondaryColorForegroundPressedEditor{};
  ColorEditor* secondaryColorForegroundDisabledEditor{};

  ColorEditor* secondaryAlternativeColorEditor{};
  ColorEditor* secondaryAlternativeColorHoveredEditor{};
  ColorEditor* secondaryAlternativeColorPressedEditor{};
  ColorEditor* secondaryAlternativeColorDisabledEditor{};

  ColorEditor* neutralColorEditor{};
  ColorEditor* neutralColorHoveredEditor{};
  ColorEditor* neutralColorPressedEditor{};
  ColorEditor* neutralColorDisabledEditor{};

  ColorEditor* semiTransparentColor1Editor{};
  ColorEditor* semiTransparentColor2Editor{};
  ColorEditor* semiTransparentColor3Editor{};
  ColorEditor* semiTransparentColor4Editor{};

  ColorEditor* backgroundColorMain1Editor{};
  ColorEditor* backgroundColorMain2Editor{};
  ColorEditor* backgroundColorMain3Editor{};
  ColorEditor* backgroundColorMain4Editor{};

  ColorEditor* borderColorEditor{};
  ColorEditor* borderColorHoveredEditor{};
  ColorEditor* borderColorPressedEditor{};
  ColorEditor* borderColorDisabledEditor{};

  ColorEditor* focusColorEditor{};

  ColorEditor* titleBarForegroundColorEditor{};
  ColorEditor* titleBarBorderColorEditor{};
  LineEdit* titleBarHeightEditor{};

  ColorEditor* shadowColor1Editor{};
  ColorEditor* shadowColor2Editor{};
  ColorEditor* shadowColor3Editor{};

  ColorEditor* statusColorErrorEditor{};
  ColorEditor* statusColorErrorHoveredEditor{};
  ColorEditor* statusColorErrorPressedEditor{};
  ColorEditor* statusColorErrorDisabledEditor{};

  ColorEditor* statusColorWarningEditor{};
  ColorEditor* statusColorWarningHoveredEditor{};
  ColorEditor* statusColorWarningPressedEditor{};
  ColorEditor* statusColorWarningDisabledEditor{};

  ColorEditor* statusColorSuccessEditor{};
  ColorEditor* statusColorSuccessHoveredEditor{};
  ColorEditor* statusColorSuccessPressedEditor{};
  ColorEditor* statusColorSuccessDisabledEditor{};

  ColorEditor* statusColorInfoEditor{};
  ColorEditor* statusColorInfoHoveredEditor{};
  ColorEditor* statusColorInfoPressedEditor{};
  ColorEditor* statusColorInfoDisabledEditor{};

  ColorEditor* statusColorForegroundEditor{};
  ColorEditor* statusColorForegroundHoveredEditor{};
  ColorEditor* statusColorForegroundPressedEditor{};
  ColorEditor* statusColorForegroundDisabledEditor{};

  LineEdit* nameEditor{};
  LineEdit* authorEditor{};
  LineEdit* versionEditor{};

  Impl(ThemeEditor& o)
    : owner(o) {}

  /// Utility to load/save from/to JSON files.
  void setupJSONLoader(QWidget* parent, QFormLayout* formLayout) {
    auto* rowLayout = new QHBoxLayout();
    const auto hSpacing = owner.style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
    rowLayout->setSpacing(hSpacing * 2);

    // 'Load' button.
    {
      auto* loadJsonButton = new QPushButton(QIcon::fromTheme("document-open"), "Load JSON file…", parent);
      loadJsonButton->setToolTip("Load a JSON file from disk.");
      loadJsonButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      rowLayout->addWidget(loadJsonButton);

      QObject::connect(loadJsonButton, &QPushButton::pressed, &owner, [this]() {
        // Get previous path.
        const auto defaultDirPath =
          QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DocumentsLocation);
        const auto defaultPath = defaultDirPath + '/' + DEFAULT_FILE_NAME;
        QSettings settings;
        const auto previousPath = settings.value(PREVIOUS_PATH_SETTINGS_KEY, defaultPath).toString();

        // Get theme from file and set it on the application.
        const auto fileName =
          QFileDialog::getOpenFileName(&owner, "Load JSON theme", previousPath, "JSON Files (*.json)");
        const auto themeOpt = Theme::fromJsonPath(fileName);
        if (!themeOpt.has_value()) {
          return;
        }

        owner.setTheme(themeOpt.value());

        // Save path to QSettings.
        settings.setValue(PREVIOUS_PATH_SETTINGS_KEY, fileName);
        settings.sync();
      });
    }

    // 'Save' button.
    {
      auto* saveJsonButton = new QPushButton(QIcon::fromTheme("document-save"), "Save JSON file…", parent);
      saveJsonButton->setToolTip("Save the current theme as JSON file to disk.");
      saveJsonButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      rowLayout->addWidget(saveJsonButton);

      QObject::connect(saveJsonButton, &QPushButton::pressed, &owner, [this]() {
        // Get previous path.
        const auto defaultDirPath =
          QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DocumentsLocation);
        const auto defaultPath = defaultDirPath + '/' + DEFAULT_FILE_NAME;
        QSettings settings;
        const auto previousPath = settings.value(PREVIOUS_PATH_SETTINGS_KEY, defaultPath).toString();

        const auto fileName =
          QFileDialog::getSaveFileName(&owner, "Save JSON theme", previousPath, "JSON Files (*.json)");
        if (!fileName.isEmpty()) {
          const auto jsonDoc = theme.toJson();
          const auto data = QByteArray(jsonDoc.toJson(QJsonDocument::JsonFormat::Indented).replace("    ", "  "));
          QFile file(fileName);
          if (file.open(QIODevice::ReadWrite)) {
            // Save to disk.
            file.resize(0);
            file.write(data);
            file.close();

            // Save path to QSettings.
            settings.setValue(PREVIOUS_PATH_SETTINGS_KEY, fileName);
            settings.sync();
          } else {
            QMessageBox::critical(&owner, "Writing Error", QString("Can't write to file:\n%1").arg(fileName),
              QMessageBox::StandardButton::Ok);
          }
        }
      });
    }

    formLayout->addRow(rowLayout);
  }

  void setupColorEditors(QFormLayout* formLayout, int vSpacing) {
    // Primary Color.
    ADD_TITLE("Primary Color");
    ADD_SUBTITLE("Background")
    ADD_DESCRIPTION("Used to highlight elements.")
    ADD_COLOR_EDITOR(primaryColor, "");
    ADD_COLOR_EDITOR(primaryColorHovered, "");
    ADD_COLOR_EDITOR(primaryColorPressed, "");
    ADD_COLOR_EDITOR(primaryColorDisabled, "");
    ADD_SUBTITLE("Foreground")
    ADD_DESCRIPTION("Text drawn over highlighted elements.");
    ADD_COLOR_EDITOR(primaryColorForeground, "");
    ADD_COLOR_EDITOR(primaryColorForegroundHovered, "");
    ADD_COLOR_EDITOR(primaryColorForegroundPressed, "");
    ADD_COLOR_EDITOR(primaryColorForegroundDisabled, "");
    ADD_SUBTITLE("Alternative")
    ADD_DESCRIPTION("Used to highlight elements over already highlighted elements.")
    ADD_COLOR_EDITOR(primaryAlternativeColor, "");
    ADD_COLOR_EDITOR(primaryAlternativeColorHovered, "");
    ADD_COLOR_EDITOR(primaryAlternativeColorPressed, "");
    ADD_COLOR_EDITOR(primaryAlternativeColorDisabled, "");

    // Secondary Color.
    ADD_TITLE("Secondary Color");
    ADD_SUBTITLE("Background");
    ADD_DESCRIPTION("A more neutral color, used for text and non-highlighted elements.");
    ADD_COLOR_EDITOR(secondaryColor, "");
    ADD_COLOR_EDITOR(secondaryColorHovered, "");
    ADD_COLOR_EDITOR(secondaryColorPressed, "");
    ADD_COLOR_EDITOR(secondaryColorDisabled, "");
    ADD_SUBTITLE("Foreground")
    ADD_DESCRIPTION("Text drawn over elements in secondary color.")
    ADD_COLOR_EDITOR(secondaryColorForeground, "");
    ADD_COLOR_EDITOR(secondaryColorForegroundHovered, "");
    ADD_COLOR_EDITOR(secondaryColorForegroundPressed, "");
    ADD_COLOR_EDITOR(secondaryColorForegroundDisabled, "");
    ADD_TITLE("Secondary Alternative Color");
    ADD_DESCRIPTION("A lighter version of the secondary color.");
    ADD_COLOR_EDITOR(secondaryAlternativeColor, "");
    ADD_COLOR_EDITOR(secondaryAlternativeColorHovered, "");
    ADD_COLOR_EDITOR(secondaryAlternativeColorPressed, "");
    ADD_COLOR_EDITOR(secondaryAlternativeColorDisabled, "");

    // Neutral color.
    ADD_TITLE("Neutral Color");
    ADD_DESCRIPTION("Used for ??");
    ADD_COLOR_EDITOR(neutralColor, "");
    ADD_COLOR_EDITOR(neutralColorHovered, "");
    ADD_COLOR_EDITOR(neutralColorPressed, "");
    ADD_COLOR_EDITOR(neutralColorDisabled, "");

    // Semi-transparent color.
    ADD_TITLE("Semi-transparent Color");
    ADD_DESCRIPTION("Used for semi-transparent elements such as scrollbars.");
    ADD_COLOR_EDITOR(semiTransparentColor1, "");
    ADD_COLOR_EDITOR(semiTransparentColor2, "");
    ADD_COLOR_EDITOR(semiTransparentColor3, "");
    ADD_COLOR_EDITOR(semiTransparentColor4, "");

    // Background Color.
    ADD_TITLE("Background Color");
    ADD_DESCRIPTION("Used for containers: windows, GroupBoxes, etc.")
    ADD_COLOR_EDITOR(backgroundColorMain1, "Main background color (window, etc).");
    ADD_COLOR_EDITOR(backgroundColorMain2, "");
    ADD_COLOR_EDITOR(backgroundColorMain3, "");
    ADD_COLOR_EDITOR(backgroundColorMain4, "");

    // Border Color.
    ADD_TITLE("Border Color");
    ADD_DESCRIPTION("Used to draw the borders: ComboBox, GroupBox's border, etc.");
    ADD_COLOR_EDITOR(borderColor, "");
    ADD_COLOR_EDITOR(borderColorHovered, "");
    ADD_COLOR_EDITOR(borderColorPressed, "");
    ADD_COLOR_EDITOR(borderColorDisabled, "");

    // Title Bar Color.
    ADD_TITLE("Title Bar Color");
    ADD_DESCRIPTION("Used to draw the title bar of a UnifiedWindow.");
    ADD_COLOR_EDITOR(titleBarForegroundColor, "");
    ADD_COLOR_EDITOR(titleBarBorderColor, "");
    ADD_INT_EDITOR(titleBarHeight, "Height of the title bar, in pixels.");

    // Focus Color.
    ADD_TITLE("Focus Color");
    ADD_COLOR_EDITOR(focusColor, "Color for the focus border.");

    // Shadow Color.
    ADD_TITLE("Shadow Color");
    ADD_COLOR_EDITOR(shadowColor1, "Shadow 1");
    ADD_COLOR_EDITOR(shadowColor2, "Shadow 2");
    ADD_COLOR_EDITOR(shadowColor3, "Shadow 3");

    // Status Color.
    ADD_TITLE("Status Color");
    ADD_DESCRIPTION("Displaying feedback to the user (success, error, etc.)")
    ADD_SUBTITLE("Error");
    ADD_COLOR_EDITOR(statusColorError, "");
    ADD_COLOR_EDITOR(statusColorErrorHovered, "");
    ADD_COLOR_EDITOR(statusColorErrorPressed, "");
    ADD_COLOR_EDITOR(statusColorErrorDisabled, "");
    ADD_SUBTITLE("Warning");
    ADD_COLOR_EDITOR(statusColorWarning, "");
    ADD_COLOR_EDITOR(statusColorWarningHovered, "");
    ADD_COLOR_EDITOR(statusColorWarningPressed, "");
    ADD_COLOR_EDITOR(statusColorWarningDisabled, "");
    ADD_SUBTITLE("Success");
    ADD_COLOR_EDITOR(statusColorSuccess, "");
    ADD_COLOR_EDITOR(statusColorSuccessHovered, "");
    ADD_COLOR_EDITOR(statusColorSuccessPressed, "");
    ADD_COLOR_EDITOR(statusColorSuccessDisabled, "");
    ADD_SUBTITLE("Info");
    ADD_COLOR_EDITOR(statusColorInfo, "");
    ADD_COLOR_EDITOR(statusColorInfoHovered, "");
    ADD_COLOR_EDITOR(statusColorInfoPressed, "");
    ADD_COLOR_EDITOR(statusColorInfoDisabled, "");
    ADD_SUBTITLE("Foreground");
    ADD_DESCRIPTION("Used to draw text over status colors.");
    ADD_COLOR_EDITOR(statusColorForeground, "");
    ADD_COLOR_EDITOR(statusColorForegroundHovered, "");
    ADD_COLOR_EDITOR(statusColorForegroundPressed, "");
    ADD_COLOR_EDITOR(statusColorForegroundDisabled, "");

    // Metadata
    ADD_TITLE("Metadata");
    ADD_METADATA_TEXT_EDITOR(name, "Name of the Qlementine theme");
    ADD_METADATA_TEXT_EDITOR(author, "Author of the Qlementine theme");
    ADD_METADATA_TEXT_EDITOR(version, "Version of the Qlementine theme");
  }

  void setupUi() {
    auto* globalLayout = new QVBoxLayout(&owner);
    owner.setLayout(globalLayout);
    const auto vSpacing = owner.style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
    const auto hSpacing = owner.style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);

    auto* formLayout = new QFormLayout();
    formLayout->setHorizontalSpacing(hSpacing * 8);
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    formLayout->setRowWrapPolicy(QFormLayout::RowWrapPolicy::DontWrapRows);

    setupJSONLoader(&owner, formLayout);
    setupColorEditors(formLayout, vSpacing);

    globalLayout->addLayout(formLayout);
  }

  // Update all the widgets.
  void updateUi() {
    UPDATE_COLOR_EDITOR(primaryColor);
    UPDATE_COLOR_EDITOR(primaryColorHovered);
    UPDATE_COLOR_EDITOR(primaryColorPressed);
    UPDATE_COLOR_EDITOR(primaryColorDisabled);

    UPDATE_COLOR_EDITOR(primaryColorForeground);
    UPDATE_COLOR_EDITOR(primaryColorForegroundHovered);
    UPDATE_COLOR_EDITOR(primaryColorForegroundPressed);
    UPDATE_COLOR_EDITOR(primaryColorForegroundDisabled);

    UPDATE_COLOR_EDITOR(secondaryColor);
    UPDATE_COLOR_EDITOR(secondaryColorHovered);
    UPDATE_COLOR_EDITOR(secondaryColorPressed);
    UPDATE_COLOR_EDITOR(secondaryColorDisabled);

    UPDATE_COLOR_EDITOR(secondaryColorForeground);
    UPDATE_COLOR_EDITOR(secondaryColorForegroundHovered);
    UPDATE_COLOR_EDITOR(secondaryColorForegroundPressed);
    UPDATE_COLOR_EDITOR(secondaryColorForegroundDisabled);

    UPDATE_COLOR_EDITOR(secondaryAlternativeColor);
    UPDATE_COLOR_EDITOR(secondaryAlternativeColorHovered);
    UPDATE_COLOR_EDITOR(secondaryAlternativeColorPressed);
    UPDATE_COLOR_EDITOR(secondaryAlternativeColorDisabled);

    UPDATE_COLOR_EDITOR(neutralColor);
    UPDATE_COLOR_EDITOR(neutralColorHovered);
    UPDATE_COLOR_EDITOR(neutralColorPressed);
    UPDATE_COLOR_EDITOR(neutralColorDisabled);

    UPDATE_COLOR_EDITOR(semiTransparentColor1);
    UPDATE_COLOR_EDITOR(semiTransparentColor2);
    UPDATE_COLOR_EDITOR(semiTransparentColor3);
    UPDATE_COLOR_EDITOR(semiTransparentColor4);

    UPDATE_COLOR_EDITOR(backgroundColorMain1);
    UPDATE_COLOR_EDITOR(backgroundColorMain2);
    UPDATE_COLOR_EDITOR(backgroundColorMain3);
    UPDATE_COLOR_EDITOR(backgroundColorMain4);

    UPDATE_COLOR_EDITOR(borderColor);
    UPDATE_COLOR_EDITOR(borderColorHovered);
    UPDATE_COLOR_EDITOR(borderColorPressed);
    UPDATE_COLOR_EDITOR(borderColorDisabled);

    UPDATE_COLOR_EDITOR(titleBarForegroundColor);
    UPDATE_COLOR_EDITOR(titleBarBorderColor);
    UPDATE_INT_EDITOR(titleBarHeight);

    UPDATE_COLOR_EDITOR(focusColor);

    UPDATE_COLOR_EDITOR(shadowColor1);
    UPDATE_COLOR_EDITOR(shadowColor2);
    UPDATE_COLOR_EDITOR(shadowColor3);

    UPDATE_COLOR_EDITOR(statusColorError);
    UPDATE_COLOR_EDITOR(statusColorErrorHovered);
    UPDATE_COLOR_EDITOR(statusColorErrorPressed);
    UPDATE_COLOR_EDITOR(statusColorErrorDisabled);

    UPDATE_COLOR_EDITOR(statusColorWarning);
    UPDATE_COLOR_EDITOR(statusColorWarningHovered);
    UPDATE_COLOR_EDITOR(statusColorWarningPressed);
    UPDATE_COLOR_EDITOR(statusColorWarningDisabled);

    UPDATE_COLOR_EDITOR(statusColorSuccess);
    UPDATE_COLOR_EDITOR(statusColorSuccessHovered);
    UPDATE_COLOR_EDITOR(statusColorSuccessPressed);
    UPDATE_COLOR_EDITOR(statusColorSuccessDisabled);

    UPDATE_COLOR_EDITOR(statusColorInfo);
    UPDATE_COLOR_EDITOR(statusColorInfoHovered);
    UPDATE_COLOR_EDITOR(statusColorInfoPressed);
    UPDATE_COLOR_EDITOR(statusColorInfoDisabled);

    UPDATE_COLOR_EDITOR(statusColorForeground);
    UPDATE_COLOR_EDITOR(statusColorForegroundHovered);
    UPDATE_COLOR_EDITOR(statusColorForegroundPressed);
    UPDATE_COLOR_EDITOR(statusColorForegroundDisabled);

    UPDATE_METADATA_TEXT_EDITOR(name);
    UPDATE_METADATA_TEXT_EDITOR(author);
    UPDATE_METADATA_TEXT_EDITOR(version);
  }

  ThemeEditor& owner;
  Theme theme;
};

ThemeEditor::ThemeEditor(QWidget* parent)
  : QWidget(parent)
  , _impl(std::make_unique<Impl>(*this)) {
  _impl->setupUi();
}

ThemeEditor::~ThemeEditor() = default;

const Theme& ThemeEditor::theme() const {
  return _impl->theme;
}

void ThemeEditor::setTheme(const Theme& theme) {
  if (theme != _impl->theme) {
    _impl->theme = theme;
    _impl->updateUi();
    Q_EMIT themeChanged(_impl->theme);
  }
}
} // namespace oclero::qlementine
