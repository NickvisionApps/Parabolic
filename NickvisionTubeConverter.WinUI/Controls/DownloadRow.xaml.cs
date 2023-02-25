using Microsoft.UI.Xaml.Controls;
using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.WinUI.Controls;

public sealed partial class DownloadRow : UserControl, IDownloadRowControl
{
    private readonly Download _download;

    public bool IsDone => _download.IsDone;

    public DownloadRow(Localizer localizer, Download download)
    {
        InitializeComponent();
        _download = download;
    }

    public async Task StartAsync(bool embedMetadata)
    {
        
    }

    public void Stop()
    {
        _download.Stop();
    }
}
