using ATL;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Notifications;
using System;
using System.IO;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class FileMetadataService : IFileMetadataService
{
    private ITranslationService _translationService;
    public INotificationService? _notificationService;

    public FileMetadataService(ITranslationService translationService, INotificationService? notificationService)
    {
        _translationService = translationService;
        _notificationService = notificationService;
    }

    public async Task<bool> RemoveSourceDataAsync(string path)
    {
        if (!File.Exists(path))
        {
            return false;
        }
        try
        {
            var track = new Track(path);
            track.Comment = string.Empty;
            track.Description = string.Empty;
            track.EncodedBy = string.Empty;
            track.Encoder = string.Empty;
            await track.SaveAsync();
            return true;
        }
        catch(Exception e)
        {
            _notificationService?.Send(new AppNotification(_translationService._("An error occured while erasing file metadata"), NotificationSeverity.Error)
            {
                Action = "error",
                ActionParam = e.ToString()
            });
            return false;
        }
    }
}
