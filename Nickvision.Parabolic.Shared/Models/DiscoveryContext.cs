using Nickvision.Desktop.Application;
using Nickvision.Desktop.Keyring;
using System;
using System.Collections.Generic;

namespace Nickvision.Parabolic.Shared.Models;

public class DiscoveryContext
{
    public int Id { get; }
    public Uri Url { get; }
    public string Title { get; }
    public Credential? Credential { get; }
    internal IReadOnlyList<Media> Media { get; }
    public List<MediaSelectionItem> Items { get; }
    public List<SelectionItem<double>> AudioBitrates { get; }
    public List<SelectionItem<Format>> AudioFormats { get; }
    public List<SelectionItem<MediaFileType>> FileTypes { get; }
    public List<SelectionItem<SubtitleLanguage>> SubtitleLanguages { get; }
    public List<SelectionItem<Format>> VideoFormats { get; }
    public List<SelectionItem<VideoResolution>> VideoResolutions { get; }

    public DiscoveryContext(int id, Uri url, string title, Credential? credential, IReadOnlyList<Media> media)
    {
        Id = id;
        Url = url;
        Title = title;
        Credential = credential;
        Media = media;
        Items = new List<MediaSelectionItem>();
        AudioBitrates = new List<SelectionItem<double>>();
        AudioFormats = new List<SelectionItem<Format>>();
        FileTypes = new List<SelectionItem<MediaFileType>>();
        SubtitleLanguages = new List<SelectionItem<SubtitleLanguage>>();
        VideoFormats = new List<SelectionItem<Format>>();
        VideoResolutions = new List<SelectionItem<VideoResolution>>();
    }
}
