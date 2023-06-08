using Nickvision.Keyring;
using NickvisionTubeConverter.Shared.Models;
using System;

namespace NickvisionTubeConverter.Shared.Controllers;

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
}