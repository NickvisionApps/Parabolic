using System;
using System.IO;
using System.Runtime.Serialization;

namespace NickvisionTubeConverter.Shared.Helpers;

/// <summary>
/// From https://github.com/pythonnet/pythonnet/issues/2008#issuecomment-1441315835
/// </summary>
public class NoopFormatter : IFormatter
{
    public object Deserialize(Stream s) => throw new NotImplementedException();
    public void Serialize(Stream s, object o) { }

    public SerializationBinder Binder { get; set; }
    public StreamingContext Context { get; set; }
    public ISurrogateSelector SurrogateSelector { get; set; }
}
