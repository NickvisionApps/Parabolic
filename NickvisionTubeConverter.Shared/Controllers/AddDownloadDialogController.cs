using NickvisionTubeConverter.Shared.Models;

namespace NickvisionTubeConverter.Shared.Controllers;

/// <summary>
/// A controller for a AddDownloadDialog
/// </summary>
public class AddDownloadDialogController
{
    /// <summary>
    /// The response of the dialog
    /// </summary>
    public string Response { get; set; }

    /// <summary>
    /// The download created by the dialog
    /// </summary>
    public Download Download { get; private set; }

    /// <summary>
    /// Gets the previously used save folder from the configuration
    /// </summary>
    /// <returns>The previously used save folder</returns>
    public string GetPreviousSaveFolder()
    {
        var path = Configuration.Current.PreviousSaveFolder;
        return System.IO.Path.Exists(path) ? path : "";
    }

    /// <summary>
    /// Gets the previously used file type (as an int) from the configuration
    /// </summary>
    /// <returns>The previously used file type (as an int)</returns>
    public int GetPreviousFileTypeAsInt()
    {
        return (int)Configuration.Current.PreviousMediaFileType;
    }

    /// <summary>
    /// Sets the download from the dialog and checks if it is valid
    /// </summary>
    /// <param name="videoUrl">The url of the video to download</param>
    /// <param name="mediaFileType">The file type to download the video as</param>
    /// <param name="saveFolder">The folder to save the download to</param>
    /// <param name="newFilename">The filename to save the download as</param>
    /// <param name="quality">The quality of the download</param>
    /// <param name="subtitles">The subtitles for the download</param>
    /// <returns>The DownloadCheckStatus</returns>
    public DownloadCheckStatus SetDownload(string videoUrl, int mediaFileType, string saveFolder, string newFileName, int quality, int subtitles)
    {
        Download = new Download(videoUrl, (MediaFileType)mediaFileType, saveFolder, newFileName, (Quality)quality, (Subtitle)subtitles);
        var checkStatus = Download.CheckStatus;
        if(checkStatus == DownloadCheckStatus.Valid)
        {
            Configuration.Current.PreviousSaveFolder = saveFolder;
            Configuration.Current.PreviousMediaFileType = (MediaFileType)mediaFileType;
            Configuration.Current.Save();
        }
        return checkStatus;
    }
}
