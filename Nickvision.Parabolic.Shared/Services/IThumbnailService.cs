using Nickvision.Parabolic.Shared.Models;
using System;
using System.IO;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public interface IThumbnailService
{
    Task<byte[]> GetImageBytesAsync(Uri url);
    Task<MemoryStream> GetImageStreamAsync(Uri url);
    void MapMedia(Media media);
}
