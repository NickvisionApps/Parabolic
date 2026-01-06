using Nickvision.Desktop.Application;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Helpers;
using Nickvision.Desktop.Keyring;
using Nickvision.Desktop.Notifications;
using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Controllers;

public class KeyringPageController
{
    private readonly INotificationService _notificationService;
    private readonly IKeyringService _keyringService;

    public ITranslationService Translator { get; }
    public ObservableCollection<SelectionItem<Credential>> Credentials { get; }

    public KeyringPageController(ITranslationService translationService, INotificationService notificationService, IKeyringService keyringService)
    {
        _notificationService = notificationService;
        _keyringService = keyringService;
        Translator = translationService;
        Credentials = new ObservableCollection<SelectionItem<Credential>>();
        foreach (var credential in _keyringService.Credentials)
        {
            Credentials.Add(new SelectionItem<Credential>(credential, credential.Name, false));
        }
    }

    public async Task AddAsync(string name, string url, string username, string password)
    {
        if (_keyringService.Credentials.Any(cred => cred.Name == name))
        {
            _notificationService.Send(new AppNotification(Translator._("A credential with that name already exists"), NotificationSeverity.Error)
            {
                Action = "error"
            });
        }
        else if (string.IsNullOrEmpty(name))
        {
            _notificationService.Send(new AppNotification(Translator._("The name of the credential cannot be empty"), NotificationSeverity.Error)
            {
                Action = "error"
            });
        }
        else if (string.IsNullOrEmpty(username) && string.IsNullOrEmpty(password))
        {
            _notificationService.Send(new AppNotification(Translator._("Either the credential username or password must be set"), NotificationSeverity.Error)
            {
                Action = "error"
            });
        }
        else
        {
            var credential = new Credential(name, username, password, Uri.Empty);
            Uri.TryCreate(url, UriKind.Absolute, out var uri);
            Credentials.Add(new SelectionItem<Credential>(credential, credential.Name, false));
            await _keyringService.AddCredentialAsync(credential);
        }
    }

    public async Task RemoveAsync(SelectionItem<Credential> credential)
    {
        Credentials.Remove(credential);
        await _keyringService.RemoveCredentialAsync(credential.Value);
    }

    public async Task UpdateAsync(string name, string url, string username, string password)
    {
        var credential = _keyringService.Credentials.FirstOrDefault(cred => cred.Name == name);
        if (credential is null)
        {
            _notificationService.Send(new AppNotification(Translator._("A credential with that name does not exist"), NotificationSeverity.Error)
            {
                Action = "error"
            });
        }
        else if (string.IsNullOrEmpty(username) && string.IsNullOrEmpty(password))
        {
            _notificationService.Send(new AppNotification(Translator._("Either the credential username or password must be set"), NotificationSeverity.Error)
            {
                Action = "error"
            });
        }
        else
        {
            Uri.TryCreate(url, UriKind.Absolute, out var uri);
            credential.Url = uri ?? Uri.Empty;
            credential.Username = username;
            credential.Password = password;
            Credentials[Credentials.IndexOf(Credentials.First(c => c.Value.Name == name))] = new SelectionItem<Credential>(credential, credential.Name, false);
            await _keyringService.UpdateCredentialAsync(credential);
        }
    }
}
