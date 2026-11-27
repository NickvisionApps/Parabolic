#ifndef POSTPROCESSINGARGUMENTDIALOG_H
#define POSTPROCESSINGARGUMENTDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QWidget>
#include <oclero/qlementine/widgets/Label.hpp>
#include <string>
#include <vector>
#include "postprocessing.h"

namespace parabolic::qt::views
{
    class PostprocessingArgumentDialog : public QDialog
    {
        Q_OBJECT

    public:
        PostprocessingArgumentDialog(const postprocessing::Argument& argument,
                                     const std::vector<std::string>& taken_names,
                                     QWidget* parent = nullptr);
        ~PostprocessingArgumentDialog() override = default;
        PostprocessingArgumentDialog(const PostprocessingArgumentDialog&) = delete;
        PostprocessingArgumentDialog(PostprocessingArgumentDialog&&) = delete;
        PostprocessingArgumentDialog& operator=(const PostprocessingArgumentDialog&) = delete;
        PostprocessingArgumentDialog& operator=(PostprocessingArgumentDialog&&) = delete;
        [[nodiscard]] postprocessing::Argument argument() const;

    private:
        Q_SLOT void translate();
        Q_SLOT void validate();
        std::vector<std::string> m_taken_names;
        oclero::qlementine::Label* m_lbl_name{ nullptr };
        QLineEdit* m_txt_name{ nullptr };
        oclero::qlementine::Label* m_lbl_step{ nullptr };
        QComboBox* m_cmb_step{ nullptr };
        oclero::qlementine::Label* m_lbl_executable{ nullptr };
        QComboBox* m_cmb_executable{ nullptr };
        oclero::qlementine::Label* m_lbl_arguments{ nullptr };
        QLineEdit* m_txt_arguments{ nullptr };
        QDialogButtonBox* m_buttons{ nullptr };
    };
}

#endif
