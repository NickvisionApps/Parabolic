using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Keyring;
using Nickvision.Desktop.Notifications;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Controllers;

public class AddDownloadDialogController
{
    private readonly IJsonFileService _jsonFileService;
    private readonly ITranslationService _translator;
    private readonly IKeyringService _keyringService;
    private readonly INotificationService _notificationService;
    private readonly IDiscoveryService _discoveryService;
    private readonly IDownloadService _downloadService;
    private readonly IDictionary<int, DiscoveryResult> _discoveryResults;
    private readonly IDictionary<int, Credential> _credentials;

    public PreviousDownloadOptions PreviousDownloadOptions { get; }

    public AddDownloadDialogController(IJsonFileService jsonFileService, ITranslationService translationService, IKeyringService keyringService, INotificationService notificationService, IDiscoveryService discoveryService, IDownloadService downloadService)
    {
        _translator = translationService;
        _jsonFileService = jsonFileService;
        _keyringService = keyringService;
        _notificationService = notificationService;
        _discoveryService = discoveryService;
        _downloadService = downloadService;
        _discoveryResults = new Dictionary<int, DiscoveryResult>();
        _credentials = new Dictionary<int, Credential>();
        PreviousDownloadOptions = _jsonFileService.Load<PreviousDownloadOptions>(PreviousDownloadOptions.Key);
    }

    public IEnumerable<string> CredentialNames
    {
        get
        {
            var names = new List<string>();
            names.EnsureCapacity(_keyringService.Credentials.Count() + 1);
            foreach (var credential in _keyringService.Credentials)
            {
                names.Add(credential.Name);
            }
            names.Sort();
            names.Insert(0, _translator._("Use manual credential"));
            return names;
        }
    }

    public ITranslationService Translator => _translator;

    public async Task<DiscoveryResult?> DiscoverAsync(Uri url, Credential? credential, CancellationToken cancellationToken)
    {
        try
        {
            var res = url.ToString().StartsWith("file://") ? await _discoveryService.GetForBatchFileAsync(url.ToString().Substring(7), credential, cancellationToken) : await _discoveryService.GetForUrlAsync(url, credential, cancellationToken);
            if(res.Media.Count == 0)
            {
                _notificationService.Send(new AppNotification(Translator._("An error occured while discovering media"), NotificationSeverity.Warning)
                {
                    Action = "error",
                    ActionParam = Translator._("No media found at the provided URL.")
                });
                return null;
            }
            _discoveryResults[res.Id] = res;
            if (credential is not null)
            {
                _credentials[res.Id] = credential;
            }
            return _discoveryResults[res.Id];
        }
        catch (TaskCanceledException)
        {
            return null;
        }
        catch (Exception e)
        {
            _notificationService.Send(new AppNotification(Translator._("An error occured while discovering media"), NotificationSeverity.Error)
            {
                Action = "error",
                ActionParam = e.Message
            });
            return null;
        }
    }

    public async Task<DiscoveryResult?> DiscoverAsync(Uri url, string credentialName, CancellationToken cancellationToken)
    {
        Credential? credential = null;
        if (!string.IsNullOrEmpty(credentialName))
        {
            credential = _keyringService.Credentials.FirstOrDefault(c => c.Name == credentialName);
        }
        return await DiscoverAsync(url, credential, cancellationToken);
    }

    public string GetDiscoveredMediaTitle(int discoveryId, int mediaIndex)
    {
        if(_discoveryResults.TryGetValue(discoveryId, out var result))
        {
            if(mediaIndex >= 0 && mediaIndex < result.Media.Count)
            {
                return result.Media[mediaIndex].Title;
            }
        }
        return string.Empty;
    }
}
