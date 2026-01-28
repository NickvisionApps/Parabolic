using Nickvision.Desktop.Application;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Helpers;
using Nickvision.Desktop.Keyring;
using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Controllers;

public class KeyringViewController
{
    private readonly IKeyringService _keyringService;

    public ITranslationService Translator { get; }
    public ObservableCollection<SelectionItem<Credential>> Credentials { get; }

    public KeyringViewController(ITranslationService translationService, IKeyringService keyringService)
    {
        _keyringService = keyringService;
        Translator = translationService;
        Credentials = new ObservableCollection<SelectionItem<Credential>>();
        foreach (var credential in _keyringService.Credentials)
        {
            Credentials.Add(new SelectionItem<Credential>(credential, credential.Name, false));
        }
    }

    public async Task<string?> AddAsync(string name, string url, string username, string password)
    {
        if (_keyringService.Credentials.Any(cred => cred.Name == name))
        {
            return Translator._("A credential with that name already exists");
        }
        else if (string.IsNullOrEmpty(name))
        {
            return Translator._("The name of the credential cannot be empty");
        }
        else if (string.IsNullOrEmpty(username) && string.IsNullOrEmpty(password))
        {
            return Translator._("Either the credential username or password must be set");
        }
        var credential = new Credential(name, username, password, Uri.Empty);
        Uri.TryCreate(url, UriKind.Absolute, out var uri);
        Credentials.Add(new SelectionItem<Credential>(credential, credential.Name, false));
        await _keyringService.AddCredentialAsync(credential);
        return null;
    }

    public async Task RemoveAsync(SelectionItem<Credential> credential)
    {
        Credentials.Remove(credential);
        await _keyringService.RemoveCredentialAsync(credential.Value);
    }

    public async Task RemoveAsync(Credential credential)
    {
        Credentials.Remove(Credentials.First(x => x.Value == credential));
        await _keyringService.RemoveCredentialAsync(credential);
    }

    public async Task<string?> UpdateAsync(string name, string url, string username, string password)
    {
        var credential = _keyringService.Credentials.FirstOrDefault(cred => cred.Name == name);
        if (credential is null)
        {
            return Translator._("A credential with that name does not exist");
        }
        else if (string.IsNullOrEmpty(username) && string.IsNullOrEmpty(password))
        {
            return Translator._("Either the credential username or password must be set");
        }
        Uri.TryCreate(url, UriKind.Absolute, out var uri);
        credential.Url = uri ?? Uri.Empty;
        credential.Username = username;
        credential.Password = password;
        Credentials[Credentials.IndexOf(Credentials.First(c => c.Value.Name == name))] = new SelectionItem<Credential>(credential, credential.Name, false);
        await _keyringService.UpdateCredentialAsync(credential);
        return null;
    }
}
