#include "views/postprocessing_argument_dialog.h"
#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QKeySequence>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QShortcut>
#include <QSignalBlocker>
#include <QString>
#include <QWidget>
#include <algorithm>
#include <oclero/qlementine/Common.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include <string>
#include <vector>
#include "app.h"
#include "application.h"
#include "helpers/translation_notifier.h"
#include "postprocessing.h"
#include "translation.h"

namespace parabolic::qt::views
{
    PostprocessingArgumentDialog::PostprocessingArgumentDialog(const postprocessing::Argument& argument,
                                                               const std::vector<std::string>& taken_names,
                                                               QWidget* parent)
        : QDialog{ parent },
          m_taken_names{ taken_names },
          m_lbl_name{ new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, this) },
          m_txt_name{ new QLineEdit(this) },
          m_lbl_step{ new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, this) },
          m_cmb_step{ new QComboBox(this) },
          m_lbl_executable{ new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, this) },
          m_cmb_executable{ new QComboBox(this) },
          m_lbl_arguments{ new oclero::qlementine::Label({}, oclero::qlementine::TextRole::Default, this) },
          m_txt_arguments{ new QLineEdit(this) },
          m_buttons{ new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this) }
    {
        setModal(true);
        setMinimumWidth(440);
        QFormLayout* layout{ new QFormLayout(this) };
        layout->addRow(m_lbl_name, m_txt_name);
        layout->addRow(m_lbl_step, m_cmb_step);
        layout->addRow(m_lbl_executable, m_cmb_executable);
        layout->addRow(m_lbl_arguments, m_txt_arguments);
        layout->addRow(m_buttons);
        translate();
        m_txt_name->setText(QString::fromStdString(argument.name()));
        m_cmb_step->setCurrentIndex(static_cast<int>(argument.step()));
        m_cmb_executable->setCurrentIndex(static_cast<int>(argument.executable()));
        m_txt_arguments->setText(QString::fromStdString(argument.args()));
        m_buttons->button(QDialogButtonBox::Ok)->setEnabled(!argument.name().empty() && !std::ranges::contains(m_taken_names, argument.name()));
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &PostprocessingArgumentDialog::translate);
        connect(new QShortcut(QKeySequence::Close, this), &QShortcut::activated, this, &QDialog::reject);
        connect(m_txt_name, &QLineEdit::textChanged, this, &PostprocessingArgumentDialog::validate);
        connect(m_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    }

    postprocessing::Argument PostprocessingArgumentDialog::argument() const
    {
        return { m_txt_name->text().trimmed().toStdString(),
                 static_cast<postprocessing::Step>(m_cmb_step->currentIndex()),
                 static_cast<postprocessing::Executable>(m_cmb_executable->currentIndex()),
                 m_txt_arguments->text().toStdString() };
    }

    void PostprocessingArgumentDialog::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        QSignalBlocker step_blocker{ m_cmb_step };
        QSignalBlocker executable_blocker{ m_cmb_executable };
        int step_index{ m_cmb_step->currentIndex() };
        int executable_index{ m_cmb_executable->currentIndex() };
        setWindowTitle(translator._("Postprocessing Argument"));
        m_lbl_name->setText(QString::fromStdString(translator._("{}:", translator._("Name"))));
        m_lbl_step->setText(QString::fromStdString(translator._("{}:", translator._("Post Processor"))));
        m_cmb_step->clear();
        m_cmb_step->addItems({ translator._("None"),
                               translator._("Merger"),
                               translator._("Modify Chapters"),
                               translator._("Split Chapters"),
                               translator._("Extract Audio"),
                               translator._("Video Remuxer"),
                               translator._("Video Converter"),
                               translator._("Metadata"),
                               translator._("Embed Subtitle"),
                               translator._("Embed Thumbnail"),
                               translator._("Subtitles Converter"),
                               translator._("Thumbnails Converter"),
                               translator._("Fixup Stretched"),
                               translator._("Fixup M4a"),
                               translator._("Fixup M3u8"),
                               translator._("Fixup Timestamp"),
                               translator._("Fixup Duration") });
        m_cmb_step->setCurrentIndex(std::max(step_index, 0));
        m_lbl_executable->setText(QString::fromStdString(translator._("{}:", translator._("Executable"))));
        m_cmb_executable->clear();
        m_cmb_executable->addItems(
                { translator._("None"), translator._("AtomicParsley"), translator._("FFmpeg"), translator._("FFprobe") });
        m_cmb_executable->setCurrentIndex(std::max(executable_index, 0));
        m_lbl_arguments->setText(QString::fromStdString(translator._("{}:", translator._("Argument"))));
    }

    void PostprocessingArgumentDialog::validate()
    {
        std::string name{ m_txt_name->text().trimmed().toStdString() };
        m_buttons->button(QDialogButtonBox::Ok)->setEnabled(!name.empty() && !std::ranges::contains(m_taken_names, name));
    }
}
