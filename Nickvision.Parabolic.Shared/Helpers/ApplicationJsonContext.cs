using Nickvision.Parabolic.Shared.Models;
using System.Text.Json.Serialization;

namespace Nickvision.Parabolic.Shared.Helpers;

[JsonSourceGenerationOptions(DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull, WriteIndented = true)]
[JsonSerializable(typeof(Configuration))]
[JsonSerializable(typeof(DownloadOptions))]
[JsonSerializable(typeof(PreviousDownloadOptions))]
public partial class ApplicationJsonContext : JsonSerializerContext
{

}
