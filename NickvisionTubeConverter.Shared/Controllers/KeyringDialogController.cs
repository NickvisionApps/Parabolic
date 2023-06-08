using Nickvision.Keyring;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.Shared.Controllers;

/// <summary>
/// Statuses for when a credential is validated
/// </summary>
[Flags]
public enum CredentialCheckStatus
{
    Valid = 1,
    EmptyName = 2,
    EmptyUsernamePassword = 4,
    InvalidUri = 8
}

/// <summary>
/// A dialog for managing a Keyring
/// </summary>
public class KeyringDialogController
{
    private readonly string _keyringName;

    /// <summary>
    /// The Keyring managed by the dialog
    /// </summary>
    internal Keyring? Keyring { get; private set; }

    /// <summary>
    /// Gets the AppInfo object
    /// </summary>
    public AppInfo AppInfo => AppInfo.Current;
    /// <summary>
    /// Whether or not the Keyring is enabled
    /// </summary>
    public bool IsEnabled => Keyring != null;
    /// <summary>
    /// Whether or not the Keyring state is valid
    /// </summary>
    public bool IsValid => !(Keyring == null && Keyring.Exists(_keyringName));

    /// <summary>
    /// Constructs a KeyringDialogController
    /// </summary>
    /// <param name="name">The name of the Keyring</param>
    /// <param name="keyring">The Keyring object</param>
    /// <exception cref="ArgumentException">Thrown if the keyring name is empty or if there is a mismatch between the name and Keyring object</exception>
    public KeyringDialogController(string name, Keyring? keyring)
    {
        if(string.IsNullOrEmpty(name))
        {
            throw new ArgumentException("Keyring name can not be empty.");
        }
        else if(keyring != null && keyring.Name != name)
        {
            throw new ArgumentException("Provided Keyring object does not match the provided keyring name.");
        }
        _keyringName = name;
        Keyring = keyring;
    }

    /// <summary>
    /// Enables the Keyring
    /// </summary>
    /// <returns>True if successful, false is Keyring already enabled or error</returns>
    public bool EnableKeyring(string password)
    {
        if(Keyring == null)
        {
            if(string.IsNullOrEmpty(password))
            {
                return false;
            }
            try
            {
                Keyring = Keyring.Access(_keyringName, password);
                return true;
            }
            catch
            {
                return false;
            }
        }
        return false;
    }

    /// <summary>
    /// Disables the Keyring and destroys its data
    /// </summary>
    /// <returns>True if successful, false is Keyring already disabled</returns>
    public bool DisableKeyring()
    {
        if(Keyring != null)
        {
            Keyring.Destroy();
            Keyring = null;
            return true;
        }
        return false;
    }

    /// <summary>
    /// Validates a credential
    /// </summary>
    /// <param name="name">The name of the credential</param>
    /// <param name="uri">The uri of the credential</param>
    /// <param name="username">The username of the credential</param>
    /// <param name="password">The password of the credential</param>
    /// <returns>CredentialCheckStatus</returns>
    public CredentialCheckStatus ValidateCredential(string name, string? uri, string username, string password)
    {
        CredentialCheckStatus result = 0;
        if(string.IsNullOrEmpty(name))
        {
            result |= CredentialCheckStatus.EmptyName;
        }
        if(string.IsNullOrEmpty(username) && string.IsNullOrEmpty(password))
        {
            result |= CredentialCheckStatus.EmptyUsernamePassword;
        }
        if(!string.IsNullOrEmpty(uri))
        {
            try
            {
                var x = new Uri(uri);
            }
            catch
            {
                result |= CredentialCheckStatus.InvalidUri;
            }
        }
        if (result != 0)
        {
            return result;
        }
        return CredentialCheckStatus.Valid;
    }

    /// <summary>
    /// Gets all credentials from the Keyring
    /// </summary>
    /// <returns>The list of Credential objects</returns>
    public async Task<List<Credential>> GetAllCredentialsAsync()
    {
        if(Keyring != null)
        {
            return await Keyring.GetAllCredentialsAsync();
        }
        return new List<Credential>();
    }

    /// <summary>
    /// Adds a credential to the Keyring
    /// </summary>
    /// <param name="name">The name of the credential</param>
    /// <param name="uri">The uri of the credential</param>
    /// <param name="username">The username of the credential</param>
    /// <param name="password">The password of the credential</param>
    /// <returns>True if successful, else false</returns>
    public async Task<bool> AddCredentialAsync(string name, string? uri, string username, string password)
    {
        if(ValidateCredential(name, uri, username, password) == CredentialCheckStatus.Valid && Keyring != null)
        {
            return await Keyring.AddCredentialAsync(new Credential(name, string.IsNullOrEmpty(uri) ? null : new Uri(uri), username, password));
        }
        return false;
    }

    /// <summary>
    /// Updates a credential in the Keyring
    /// </summary>
    /// <param name="id">The id of the credential</param>
    /// <param name="name">The name of the credential</param>
    /// <param name="uri">The uri of the credential</param>
    /// <param name="username">The username of the credential</param>
    /// <param name="password">The password of the credential</param>
    /// <returns>True if successful, else false</returns>
    public async Task<bool> UpdateCredentialAsync(int id, string name, string? uri, string username, string password)
    {
        if(ValidateCredential(name, uri, username, password) == CredentialCheckStatus.Valid && Keyring != null)
        {
            var credential = await Keyring.LookupCredentialAsync(id);
            if(credential != null)
            {
                credential.Name = name;
                credential.Uri = string.IsNullOrEmpty(uri) ? null : new Uri(uri);
                credential.Username = username;
                credential.Password = password;
                return await Keyring.UpdateCredentialAsync(credential);
            }
        }
        return false;
    }

    /// <summary>
    /// Deletes a credential in the Keyring
    /// </summary>
    /// <param name="id">The id of the credential</param>
    /// <returns>True if successful, else false</returns>
    public async Task<bool> DeleteCredentialAsync(int id)
    {
        if(Keyring != null)
        {
            return await Keyring.DeleteCredentialAsync(id);
        }
        return false;
    }
}