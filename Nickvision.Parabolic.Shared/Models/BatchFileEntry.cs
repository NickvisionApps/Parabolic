using System;

namespace Nickvision.Parabolic.Shared.Models;

public class BatchFileEntry
{
    public Uri Url { get; init; }
    public string SuggestedSaveFolder { get; set; }
    public string SuggestedFilename { get; set; }

    public BatchFileEntry(Uri url)
    {
        Url = url;
        SuggestedSaveFolder = string.Empty;
        SuggestedFilename = string.Empty;
    }
}
