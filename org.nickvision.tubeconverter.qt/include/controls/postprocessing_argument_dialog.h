#ifndef POSTPROCESSINGARGUMENTDIALOG_H
#define POSTPROCESSINGARGUMENTDIALOG_H

#include <QDialog>
#include <QWidget>
#include <string>
#include <vector>
#include "postprocessing.h"

namespace Ui
{
    class PostprocessingArgumentDialog;
}

namespace parabolic::qt::controls
{
    class PostprocessingArgumentDialog : public QDialog
    {
        Q_OBJECT

    public:
        PostprocessingArgumentDialog(const postprocessing::Argument& argument,
                                     const std::vector<std::string>& taken_names,
                                     QWidget* parent = nullptr);
        ~PostprocessingArgumentDialog() override;
        PostprocessingArgumentDialog(const PostprocessingArgumentDialog&) = delete;
        PostprocessingArgumentDialog(PostprocessingArgumentDialog&&) = delete;
        PostprocessingArgumentDialog& operator=(const PostprocessingArgumentDialog&) = delete;
        PostprocessingArgumentDialog& operator=(PostprocessingArgumentDialog&&) = delete;
        [[nodiscard]] postprocessing::Argument argument() const;

    private:
        Q_SLOT void translate();
        Q_SLOT void validate();
        std::vector<std::string> m_taken_names;
        Ui::PostprocessingArgumentDialog* m_ui{ nullptr };
    };
}

#endif
