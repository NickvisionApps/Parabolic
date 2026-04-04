using Nickvision.Desktop.Application;
using Nickvision.Parabolic.Shared.Models;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace Nickvision.Parabolic.Shared.Helpers;

[JsonSourceGenerationOptions(DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull, WriteIndented = true)]
[JsonSerializable(typeof(AppVersion))]
[JsonSerializable(typeof(DownloadOptions))]
[JsonSerializable(typeof(DownloaderOptions))]
[JsonSerializable(typeof(JsonElement))]
[JsonSerializable(typeof(PreviousDownloadOptions))]
[JsonSerializable(typeof(WindowGeometry))]
public partial class ApplicationJsonContext : JsonSerializerContext
{

}
