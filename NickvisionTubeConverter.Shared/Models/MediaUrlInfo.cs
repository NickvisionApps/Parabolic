﻿using Nickvision.Aura;
using NickvisionTubeConverter.Shared.Helpers;
using Python.Runtime;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A model of information about a media url
/// </summary>
public class MediaUrlInfo
{
    private bool _tryVideo;

    /// <summary>
    /// The media url
    /// </summary>
    public string Url { get; init; }
    /// <summary>
    /// All media found under a media url
    /// </summary>
    public List<MediaInfo> MediaList { get; init; }
    /// <summary>
    /// The available video resolutions
    /// </summary>
    public List<VideoResolution> VideoResolutions { get; init; }
    /// <summary>
    /// The list of audio language codes
    /// </summary>
    public List<string> AudioLanguages { get; init; }

    /// <summary>
    /// Constructs a MediaUrlInfo
    /// </summary>
    /// <param name="url">The url of the media</param>
    private MediaUrlInfo(string url)
    {
        _tryVideo = false;
        Url = url;
        MediaList = new List<MediaInfo>();
        VideoResolutions = new List<VideoResolution>();
        AudioLanguages = new List<string>();
    }

    /// <summary>
    /// Gets a MediaUrlInfo from a url string
    /// </summary>
    /// <param name="url">The media url string</param>
    /// <param name="username">A username for the website (if available)</param>
    /// <param name="password">A password for the website (if available)</param>
    /// <param name="options">DownloadOptions</param>
    /// <returns>A MediaUrlInfo object. Null if url invalid</returns>
    public static async Task<MediaUrlInfo?> GetAsync(string url, string? username, string? password, DownloadOptions options)
    {
        var pathToOutput = $"{UserDirectories.ApplicationCache}{Path.DirectorySeparatorChar}output.log";
        dynamic outFile = PythonHelpers.SetConsoleOutputFilePath(pathToOutput);
        return await Task.Run(() =>
        {
            try
            {
                var mediaUrlInfo = new MediaUrlInfo(url);
                using (Py.GIL())
                {
                    dynamic ytdlp = Py.Import("yt_dlp");
                    var ytOpt = new Dictionary<string, dynamic>() {
                        { "quiet", true },
                        { "merge_output_format", null },
                        { "windowsfilenames", options.LimitCharacters },
                        { "ignoreerrors", true },
                        { "extract_flat", "in_playlist" }
                    };
                    string? metadataLang = null;
                    if (Download.YoutubeLangCodes.Contains(CultureInfo.CurrentCulture.Name))
                    {
                        metadataLang = CultureInfo.CurrentCulture.Name;
                    }
                    else if (Download.YoutubeLangCodes.Contains(CultureInfo.CurrentCulture.TwoLetterISOLanguageName))
                    {
                        metadataLang = CultureInfo.CurrentCulture.TwoLetterISOLanguageName;
                    }
                    if (!string.IsNullOrEmpty(metadataLang))
                    {
                        var youtubeLang = new PyList();
                        youtubeLang.Append(new PyString(metadataLang));
                        var youtubeExtractorOpt = new PyDict();
                        youtubeExtractorOpt["lang"] = youtubeLang;
                        var extractorArgs = new PyDict();
                        extractorArgs["youtube"] = youtubeExtractorOpt;
                        ytOpt.Add("extractor_args", extractorArgs);
                    }
                    if (!string.IsNullOrEmpty(options.ProxyUrl))
                    {
                        ytOpt.Add("proxy", new PyString(options.ProxyUrl));
                    }
                    if (!string.IsNullOrEmpty(username))
                    {
                        ytOpt.Add("username", username);
                    }
                    if (!string.IsNullOrEmpty(password))
                    {
                        ytOpt.Add("password", password);
                    }
                    var yt = ytdlp.YoutubeDL(ytOpt);
                    PyDict? mediaInfo = yt.extract_info(url, download: false);
                    if (mediaInfo == null)
                    {
                        outFile.close();
                        return null;
                    }
                    if (mediaInfo.HasKey("entries"))
                    {
                        var i = 1u;
                        foreach (var e in mediaInfo["entries"].As<PyList>())
                        {
                            if (e.IsNone())
                            {
                                continue;
                            }
                            mediaUrlInfo.ParseFromPyDict(yt, e.As<PyDict>(), i, url, options.LimitCharacters);
                            i++;
                        }
                    }
                    else
                    {
                        mediaUrlInfo.ParseFromPyDict(yt, mediaInfo, 0, url, options.LimitCharacters);
                    }
                    outFile.close();
                }
                return mediaUrlInfo;
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                using (Py.GIL())
                {
                    outFile.close();
                }
                return null;
            }
        });
    }

    /// <summary>
    /// Parses formats from media info
    /// </summary>
    private void ParseFormats(PyDict mediaInfo)
    {
        foreach (var f in mediaInfo["formats"].As<PyList>())
        {
            var format = f.As<PyDict>();
            if (format.HasKey("vcodec"))
            {
                if (format["vcodec"].As<string>() == "none")
                {
                    if (format.HasKey("language"))
                    {
                        if (!format["language"].IsNone() && !AudioLanguages.Contains(format["language"].As<string>()))
                        {
                            AudioLanguages.Add(format["language"].As<string>());
                        }
                    }
                }
                else if (format.HasKey("resolution"))
                {
                    try
                    {
                        var resolution = VideoResolution.Parse(format["resolution"].As<string>());
                        if (resolution != null && !VideoResolutions.Contains(resolution))
                        {
                            VideoResolutions.Add(resolution);
                        }
                    }
                    catch { }
                }
            }
        }
        if (VideoResolutions.Count > 0)
        {
            VideoResolutions.Add(VideoResolution.Best);
            VideoResolutions.Sort((a, b) => b.CompareTo(a));
        }
        AudioLanguages.Sort();
    }

    /// <summary>
    /// Adds data retrieved from yt-dlp to MediaInfo
    /// </summary>
    /// <param name="yt">The YouttubeDL object</param>
    /// <param name="mediaInfo">Python dictionary with media info</param>
    /// <param name="playlistPosition">Position in playlist starting with 1, or 0 if not in playlist</param>
    /// <param name="defaultUrl">A default URL to use if none available</param>
    /// <param name="limitChars">Whether or not to limit characters to those only supported by Windows</param>
    private void ParseFromPyDict(dynamic yt, PyDict mediaInfo, uint playlistPosition, string defaultUrl, bool limitChars)
    {
        var title = mediaInfo.HasKey("title") ? (mediaInfo["title"].As<string?>() ?? "Media") : "Media";
        IEnumerable<char> invalidChars = Path.GetInvalidFileNameChars();
        if (limitChars)
        {
            invalidChars = invalidChars.Union(new char[] { '"', '<', '>', ':', '\\', '/', '|', '?', '*' });
        }
        foreach (var c in invalidChars)
        {
            title = title.Replace(c, '_');
        }
        if (mediaInfo.HasKey("formats"))
        {
            ParseFormats(mediaInfo);
        }
        if (VideoResolutions.Count == 0 && mediaInfo.HasKey("url") && !_tryVideo)
        {
            var tempUrl = mediaInfo["url"].As<string>();
            PyDict? tempInfo = yt.extract_info(tempUrl, download: false);
            if (tempInfo != null)
            {
                ParseFormats(tempInfo);
                _tryVideo = true;
            }
        }
        if (VideoResolutions.Count == 0 && mediaInfo.HasKey("width") && mediaInfo.HasKey("height"))
        {
            VideoResolutions.Add(VideoResolution.Best);
            var res = VideoResolution.Parse($"{mediaInfo["width"].As<int>}x{mediaInfo["height"].As<int>()}");
            if (res != null)
            {
                VideoResolutions.Add(res);
            }
        }
        if (VideoResolutions.Count == 0 && mediaInfo.HasKey("video_ext") && mediaInfo["video_ext"].As<string>() != "none")
        {
            VideoResolutions.Add(VideoResolution.Best);
        }
        var duration = 0.0;
        try
        {
            duration = mediaInfo["duration"].As<double>();
        }
        catch { }
        var url = defaultUrl;
        if (mediaInfo.HasKey("webpage_url"))
        {
            url = mediaInfo["webpage_url"].As<string>();
        }
        if ((!mediaInfo.HasKey("webpage_url") || url.StartsWith("https://www.instagram.com")) && mediaInfo.HasKey("url"))
        {
            url = mediaInfo["url"].As<string>();
        }
        MediaList.Add(new MediaInfo(url, title, duration, playlistPosition));
    }
}
