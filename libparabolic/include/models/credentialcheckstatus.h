#ifndef CREDENTIALCHECKSTATUS_H
#define CREDENTIALCHECKSTATUS_H

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief Flags to describe the status of a validated credential.
     */
    enum class CredentialCheckStatus
    {
        Valid = 1, ///< The credential is valid.
        EmptyName = 2, ///< The credential has an empty name.
        EmptyUsernamePassword = 4, ///< The credential has an empty username or password.
        InvalidUri = 8, ///< The credential has an invalid URI.
        ExistingName = 16, ///< The credential has a name that already exists.
        DatabaseError = 32, ///< There was an error with the database.
    };
}

#endif //CREDENTIALCHECKSTATUS_H