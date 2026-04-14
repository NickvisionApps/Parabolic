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
    private readonly ITranslationService _translationService;
    private readonly ObservableCollection<SelectionItem<Credential>> _credentials;

    public KeyringViewController(IKeyringService keyringService, ITranslationService translationService)
    {
        _keyringService = keyringService;
        _translationService = translationService;
        _credentials = new ObservableCollection<SelectionItem<Credential>>();
    }

    public async Task<string?> AddAsync(string name, string url, string username, string password)
    {
        if ((await GetAllAsync()).Any(cred => cred.Value.Name == name))
        {
            return _translationService._("A credential with that name already exists");
        }
        else if (string.IsNullOrEmpty(name))
        {
            return _translationService._("The name of the credential cannot be empty");
        }
        else if (string.IsNullOrEmpty(username) && string.IsNullOrEmpty(password))
        {
            return _translationService._("Either the credential username or password must be set");
        }
        Uri.TryCreate(url, UriKind.Absolute, out var uri);
        var credential = new Credential(name, username, password, uri ?? Uri.Empty);
        _credentials.Add(new SelectionItem<Credential>(credential, credential.Name, false));
        await _keyringService.AddCredentialAsync(credential);
        return null;
    }

    public async Task<ObservableCollection<SelectionItem<Credential>>> GetAllAsync()
    {
        if (_credentials.Count == 0)
        {
            foreach (var credential in await _keyringService.GetAllCredentialAsync())
            {
                _credentials.Add(new SelectionItem<Credential>(credential, credential.Name, false));
            }
        }
        return _credentials;
    }

    public async Task RemoveAsync(SelectionItem<Credential> credential)
    {
        _credentials.Remove(credential);
        await _keyringService.DeleteCredentialAsync(credential.Value);
    }

    public async Task RemoveAsync(Credential credential)
    {
        _credentials.Remove(_credentials.First(x => x.Value == credential));
        await _keyringService.DeleteCredentialAsync(credential);
    }

    public async Task<string?> UpdateAsync(string name, string url, string username, string password)
    {
        var credential = _credentials.FirstOrDefault(cred => cred.Value.Name == name)?.Value;
        if (credential is null)
        {
            return _translationService._("A credential with that name does not exist");
        }
        else if (string.IsNullOrEmpty(username) && string.IsNullOrEmpty(password))
        {
            return _translationService._("Either the credential username or password must be set");
        }
        Uri.TryCreate(url, UriKind.Absolute, out var uri);
        credential.Url = uri ?? Uri.Empty;
        credential.Username = username;
        credential.Password = password;
        _credentials[_credentials.IndexOf(_credentials.First(c => c.Value.Name == name))] = new SelectionItem<Credential>(credential, credential.Name, false);
        await _keyringService.UpdateCredentialAsync(credential);
        return null;
    }
}
