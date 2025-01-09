#ifndef KEYRINGROW_H
#define KEYRINGROW_H

#include <QString>
#include <QWidget>
#include <libnick/keyring/credential.h>

namespace Ui { class KeyringRow; }

namespace Nickvision::TubeConverter::Qt::Controls
{
    /**
     * @brief A row that displays a credential.
     */
    class KeyringRow : public QWidget
    {
    Q_OBJECT

    public:
        /**
         * @brief Constructs a KeyringRow.
         * @param credential The credential to display
         * @param parent The parent widget 
         */
        KeyringRow(const Keyring::Credential& credential, QWidget* parent = nullptr);
        /**
         * @brief Destructs a KeyringRow.
         */
        ~KeyringRow();

    Q_SIGNALS:
        /**
         * @brief Emitted when the edit button is clicked.
         * @param name The name of the credential
         */
        void editCredential(const QString& name);
        /**
         * @brief Emitted when the delete button is clicked.
         * @param name The name of the credential
         */
        void deleteCredential(const QString& name);

    private:
        Ui::KeyringRow* m_ui;
        QString m_name;
    };
}

#endif //KEYRINGROW_H