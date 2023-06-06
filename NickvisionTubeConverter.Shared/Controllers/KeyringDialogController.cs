using NickvisionTubeConverter.Shared.Models;

namespace NickvisionTubeConverter.Shared.Controllers;

public class KeyringDialogController
{
    /// <summary>
    /// Gets the AppInfo object
    /// </summary>
    public AppInfo AppInfo => AppInfo.Current;
    
    public KeyringDialogController()
    {
        
    }
}