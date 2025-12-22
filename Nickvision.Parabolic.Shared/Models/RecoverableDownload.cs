using System.Text.Json;

namespace Nickvision.Parabolic.Shared.Models;

public class RecoverableDownload
{
    public int Id { get; }
    public DownloadOptions Options { get; }
    public bool CredentialRequired { get; }

    public RecoverableDownload(int id, DownloadOptions options)
    {
        Id = id;
        Options = JsonSerializer.Deserialize<DownloadOptions>(JsonSerializer.Serialize(options))!;
        CredentialRequired = Options.Credential is not null;
        Options.Credential = null;
    }

    public RecoverableDownload(int id, DownloadOptions options, bool credentialRequired)
    {
        Id = id;
        Options = options;
        CredentialRequired = credentialRequired;
    }
}
