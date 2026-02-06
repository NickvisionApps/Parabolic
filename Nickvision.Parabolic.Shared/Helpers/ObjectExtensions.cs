using System.Text.Json;

namespace Nickvision.Parabolic.Shared.Helpers;

public static class ObjectExtensions
{
    extension<T>(T obj)
    {
        public T DeepCopy() => JsonSerializer.Deserialize<T>(JsonSerializer.Serialize(obj))!;
    }
}
