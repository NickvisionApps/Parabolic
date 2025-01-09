#include "controls/keyringrow.h"
#include "ui_keyringrow.h"
#include <libnick/localization/gettext.h>

using namespace Nickvision::Keyring;

namespace Nickvision::TubeConverter::Qt::Controls
{
    KeyringRow::KeyringRow(const Credential& credential, QWidget* parent)
        : QWidget{ parent },
        m_ui{ new Ui::KeyringRow() },
        m_name{ QString::fromStdString(credential.getName()) }
    {
        m_ui->setupUi(this);
        //Localize strings
        m_ui->lblName->setText(m_name);
        m_ui->lblUrl->setText(QString::fromStdString(credential.getUri()));
        m_ui->btnEdit->setText(_("Edit"));
        m_ui->btnDelete->setText(_("Delete"));
        //Signals
        connect(m_ui->btnEdit, &QPushButton::clicked, [this]() { Q_EMIT editCredential(m_name); });
        connect(m_ui->btnDelete, &QPushButton::clicked, [this]() { Q_EMIT deleteCredential(m_name); });
    }

    KeyringRow::~KeyringRow()
    {
        delete m_ui;
    }
}