using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Vanara.PInvoke;
using static Vanara.PInvoke.AdvApi32;

namespace Nickvision.Parabolic.Shared.Controllers;

public class AddDownloadDialogController
{
    private readonly ITranslationService _translator;
    private readonly IKeyringService _keyringService;
    private readonly IDiscoveryService _discoveryService;
    private readonly IDownloadService _downloadService;

    public AddDownloadDialogController(ITranslationService translationService, IKeyringService keyringService, IDiscoveryService discoveryService, IDownloadService downloadService)
    {
        _translator = translationService;
        _keyringService = keyringService;
        _discoveryService = discoveryService;
        _downloadService = downloadService;
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
            if (url.ToString().StartsWith("file://"))
            {
                return await _discoveryService.GetForBatchFileAsync(url.ToString().Substring(7), credential, cancellationToken);
            }
            else
            {
                return await _discoveryService.GetForUrlAsync(url, credential, cancellationToken);
            }
        }
        catch (TaskCanceledException)
        {
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
}
