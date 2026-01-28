using Nickvision.Desktop.Keyring;
using System;

namespace Nickvision.Parabolic.Shared.Events;

public class DownloadCredentialRequiredEventArgs : EventArgs
{
    public Credential Credential { get; }

    public DownloadCredentialRequiredEventArgs(string title, Uri url)
    {
        Credential = new Credential(title, string.Empty, string.Empty, url);
    }
}
