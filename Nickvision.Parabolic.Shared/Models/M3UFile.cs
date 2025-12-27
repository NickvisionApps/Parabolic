using Nickvision.Parabolic.Shared.Helpers;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Models;

public class M3UFile
{
    private readonly PathType _pathType;
    private string _content;

    public string Title { get; }

    public M3UFile(string title, PathType pathType)
    {
        _pathType = pathType;
        Title = title;
        _content = "#EXTM3U\n";
        if (!string.IsNullOrEmpty(Title))
        {
            _content += $"#PLAYLIST:{Title}\n";
        }
    }

    public bool Add(DownloadOptions options)
    {
        if (options.FileType.IsGeneric)
        {
            return false;
        }
        var path = Path.Combine(options.SaveFolder, $"{options.SaveFilename}{options.FileType.DotExtension}");
        _content += $"{_pathType switch
        {
            PathType.Relative => Path.GetRelativePath(options.SaveFolder, path),
            _ => path
        }}\n";
        return true;
    }

    public bool Add(IReadOnlyCollection<DownloadOptions> options)
    {
        var success = true;
        foreach (var option in options)
        {
            success &= Add(option);
        }
        return success;
    }

    public async Task WriteAsync(string path)
    {
        if (Path.GetExtension(path).ToLower() != ".m3u")
        {
            path += ".m3u";
        }
        await File.WriteAllTextAsync(path, _content, Encoding.UTF8);
    }
}
