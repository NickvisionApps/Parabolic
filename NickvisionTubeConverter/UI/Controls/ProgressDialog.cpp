#include "ProgressDialog.h"
#include <thread>
#include "../../Helpers/ThemeHelpers.h"

using namespace NickvisionTubeConverter::Helpers;

namespace NickvisionTubeConverter::UI::Controls
{
	ProgressDialog::ProgressDialog(QWidget* parent, const QString& description, const std::function<void()>& work) : QDialog{ parent, Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint }, m_isFinished{ false }, m_work{ work }
	{
		//==UI==//
		m_ui.setupUi(this);
		m_ui.lblDescription->setText(description);
		//==Theme==//
		ThemeHelpers::applyWin32Theme(this);
	}

	int ProgressDialog::exec()
	{
		if (!m_isFinished)
		{
			std::jthread threadWorker([&]() {
				m_work();
				m_isFinished = true;
				close();
			});
			return QDialog::exec();
		}
		return 0;
	}
}
