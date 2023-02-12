using System.Threading.Tasks;

namespace NickvisionTubeConverter.Shared.Controls;

/// <summary>
/// A contract for a download row control
/// </summary>
public interface IDownloadRowControl
{
    public Task StartAsync();
}
