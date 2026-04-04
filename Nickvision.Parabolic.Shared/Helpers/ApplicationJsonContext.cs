using Nickvision.Desktop.Application;
using Nickvision.Parabolic.Shared.Models;
using System.Collections.Generic;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace Nickvision.Parabolic.Shared.Helpers;

[JsonSourceGenerationOptions(DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull, WriteIndented = true)]
[JsonSerializable(typeof(AppVersion))]
[JsonSerializable(typeof(Dictionary<MediaFileType, string>))]
[JsonSerializable(typeof(DownloadOptions))]
[JsonSerializable(typeof(JsonElement))]
[JsonSerializable(typeof(List<PostProcessorArgument>))]
[JsonSerializable(typeof(List<SubtitleLanguage>))]
[JsonSerializable(typeof(VideoResolution))]
[JsonSerializable(typeof(WindowGeometry))]
public partial class ApplicationJsonContext : JsonSerializerContext
{

}
