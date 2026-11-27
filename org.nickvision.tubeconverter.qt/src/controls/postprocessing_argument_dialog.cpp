#include "controls/postprocessing_argument_dialog.h"
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QKeySequence>
#include <QLineEdit>
#include <QObject>
#include <QPalette>
#include <QPushButton>
#include <QShortcut>
#include <QSignalBlocker>
#include <QString>
#include <QWidget>
#include <algorithm>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <string>
#include <vector>
#include "app.h"
#include "application.h"
#include "helpers/translation_notifier.h"
#include "postprocessing.h"
#include "translation.h"
#include "ui_postprocessing_argument_dialog.h"

namespace parabolic::qt::controls
{
    PostprocessingArgumentDialog::PostprocessingArgumentDialog(const postprocessing::Argument& argument,
                                                               const std::vector<std::string>& taken_names,
                                                               QWidget* parent)
        : QDialog{ parent },
          m_taken_names{ taken_names },
          m_ui{ new Ui::PostprocessingArgumentDialog() }
    {
        m_ui->setupUi(this);
        QPalette error_palette{ m_ui->lbl_error->palette() };
        error_palette.setColor(QPalette::WindowText, oclero::qlementine::appStyle()->theme().statusColorError);
        m_ui->lbl_error->setPalette(error_palette);
        translate();
        m_ui->txt_name->setText(QString::fromStdString(argument.name()));
        m_ui->cmb_step->setCurrentIndex(static_cast<int>(argument.step()));
        m_ui->cmb_executable->setCurrentIndex(static_cast<int>(argument.executable()));
        m_ui->txt_arguments->setText(QString::fromStdString(argument.args()));
        validate();
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &PostprocessingArgumentDialog::translate);
        connect(new QShortcut(QKeySequence::Close, this), &QShortcut::activated, this, &QDialog::reject);
        connect(m_ui->txt_name, &QLineEdit::textChanged, this, &PostprocessingArgumentDialog::validate);
    }

    PostprocessingArgumentDialog::~PostprocessingArgumentDialog()
    {
        delete m_ui;
    }

    postprocessing::Argument PostprocessingArgumentDialog::argument() const
    {
        return { m_ui->txt_name->text().trimmed().toStdString(),
                 static_cast<postprocessing::Step>(m_ui->cmb_step->currentIndex()),
                 static_cast<postprocessing::Executable>(m_ui->cmb_executable->currentIndex()),
                 m_ui->txt_arguments->text().toStdString() };
    }

    void PostprocessingArgumentDialog::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        QSignalBlocker step_blocker{ m_ui->cmb_step };
        QSignalBlocker executable_blocker{ m_ui->cmb_executable };
        int step_index{ m_ui->cmb_step->currentIndex() };
        int executable_index{ m_ui->cmb_executable->currentIndex() };
        setWindowTitle(translator._("Postprocessing Argument"));
        m_ui->lbl_name->setText(QString::fromStdString(translator._("{}:", translator._("Name"))));
        m_ui->lbl_step->setText(QString::fromStdString(translator._("{}:", translator._("Post Processor"))));
        m_ui->cmb_step->clear();
        m_ui->cmb_step->addItems({ translator._("None"),
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
        m_ui->cmb_step->setCurrentIndex(std::max(step_index, 0));
        m_ui->lbl_executable->setText(QString::fromStdString(translator._("{}:", translator._("Executable"))));
        m_ui->cmb_executable->clear();
        m_ui->cmb_executable->addItems(
                { translator._("None"), translator._("AtomicParsley"), translator._("FFmpeg"), translator._("FFprobe") });
        m_ui->cmb_executable->setCurrentIndex(std::max(executable_index, 0));
        m_ui->lbl_arguments->setText(QString::fromStdString(translator._("{}:", translator._("Argument"))));
        validate();
    }

    void PostprocessingArgumentDialog::validate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        std::string name{ m_ui->txt_name->text().trimmed().toStdString() };
        bool is_taken{ std::ranges::contains(m_taken_names, name) };
        QString error;
        if (name.empty() && m_ui->txt_name->isModified())
        {
            error = translator._("A name is required.");
        }
        else if (is_taken)
        {
            error = translator._("An argument with this name already exists.");
        }
        bool was_visible{ m_ui->lbl_error->isVisibleTo(this) };
        m_ui->lbl_error->setText(error);
        m_ui->lbl_error->setVisible(!error.isEmpty());
        if (was_visible != !error.isEmpty())
        {
            adjustSize();
        }
        m_ui->buttons->button(QDialogButtonBox::Ok)->setEnabled(!name.empty() && !is_taken);
    }
}
