using ATL;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Helpers;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Helpers;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Text;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Models;

public partial class Download : IDisposable
{
    private static int _nextId;

    private readonly IConfigurationService _configurationService;
    private readonly ITranslationService _translationService;
    private readonly IYtdlpExecutableService _ytdlpExecutableService;
    private readonly StringBuilder _logBuilder;
    private bool _removeSourceData;
    private Process? _process;
    private int _progressSkipCounter;

    public int Id { get; }
    public DownloadOptions Options { get; }
    public string FilePath { get; private set; }
    public DownloadStatus Status { get; private set; }
    public string Log => _logBuilder.ToString();

    public event EventHandler<DownloadCompletedEventArgs>? Completed;
    public event EventHandler<DownloadProgressChangedEventArgs>? ProgressChanged;

    static Download()
    {
        _nextId = 0;
    }

    public Download(IConfigurationService configurationService, ITranslationService translationService, IYtdlpExecutableService ytdlpExecutableService, DownloadOptions options)
    {
        _configurationService = configurationService;
        _translationService = translationService;
        _ytdlpExecutableService = ytdlpExecutableService;
        _logBuilder = new StringBuilder();
        _removeSourceData = false;
        _process = null;
        _progressSkipCounter = 0;
        Id = _nextId++;
        Options = options;
        FilePath = Path.Combine(Options.SaveFolder, $"{Options.SaveFilename}{Options.FileType.DotExtension}");
        Status = DownloadStatus.Queued;
    }

    ~Download()
    {
        Dispose(false);
    }

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    public void Pause()
    {
        if (Status != DownloadStatus.Running)
        {
            return;
        }
        Status = DownloadStatus.Paused;
        _process?.Suspend();
    }

    public void Resume()
    {
        if (Status != DownloadStatus.Paused)
        {
            return;
        }
        Status = DownloadStatus.Running;
        _process?.Resume();
        ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, ReadOnlyMemory<char>.Empty, double.NaN, 0.0, 0));
    }

    public void Start()
    {
        if (Status == DownloadStatus.Running || Status == DownloadStatus.Paused)
        {
            return;
        }
        if (File.Exists(FilePath) && !_configurationService.OverwriteExistingFiles)
        {
            var log = _translationService?._("The file already exists and overwriting is disabled.") ?? "The file already exists and overwriting is disabled.";
            _logBuilder.AppendLine(log);
            Status = DownloadStatus.Error;
            ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, log.AsMemory(), double.NaN, 0.0, 0));
            Completed?.Invoke(this, new DownloadCompletedEventArgs(Id, Status, FilePath, log.AsMemory(), false));
            return;
        }
        _removeSourceData = _configurationService.RemoveSourceData;
        _process = _ytdlpExecutableService.GetDownloadProcess(Options);
        Status = DownloadStatus.Running;
        _process.Exited += Process_Exited;
        _process.OutputDataReceived += Process_OutputDataReceived;
        _process.ErrorDataReceived += Process_OutputDataReceived;
        _process.Start();
        _process.SetAsParentProcess();
        _process.BeginOutputReadLine();
        _process.BeginErrorReadLine();
        ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, (_translationService?._("Starting download...") ?? "Starting download...").AsMemory(), double.NaN, 0.0, 0));
    }

    public void Stop()
    {
        if (Status != DownloadStatus.Running)
        {
            return;
        }
        Status = DownloadStatus.Stopped;
        _process?.Kill(true);
    }

    private void Dispose(bool disposing)
    {
        if (!disposing)
        {
            return;
        }
        if (_process is not null && !_process.HasExited)
        {
            _process?.Kill(true);
            _process?.WaitForExit();
        }
    }

    private async void Process_Exited(object? sender, EventArgs e)
    {
        if (Status != DownloadStatus.Stopped)
        {
            Status = _process?.ExitCode == 0 ? DownloadStatus.Success : DownloadStatus.Error;
        }
        if (Status == DownloadStatus.Success)
        {
            try
            {
                var finalPath = string.Empty;
                var log = _logBuilder.ToString();
                var endIndex = log.Length;
                for (var i = 0; i < 2 && endIndex > 0; i++)
                {
                    var startIndex = log.LastIndexOf('\n', endIndex - 1);
                    var line = (startIndex == -1 ? log[..endIndex] : log[(startIndex + 1)..endIndex]).Trim('\r');
                    if (!string.IsNullOrWhiteSpace(line))
                    {
                        finalPath = line;
                        break;
                    }
                    endIndex = startIndex == -1 ? 0 : startIndex;
                }
                if (!string.IsNullOrEmpty(finalPath) && File.Exists(finalPath))
                {
                    FilePath = finalPath;
                    if (_removeSourceData)
                    {
                        var track = new Track(FilePath);
                        track.Comment = string.Empty;
                        track.Description = string.Empty;
                        track.EncodedBy = string.Empty;
                        track.Encoder = string.Empty;
                        if (track.AdditionalFields.ContainsKey("comment"))
                        {
                            track.AdditionalFields.Remove("comment");
                        }
                        if (track.AdditionalFields.ContainsKey("COMMENT"))
                        {
                            track.AdditionalFields.Remove("COMMENT");
                        }
                        if (track.AdditionalFields.ContainsKey("description"))
                        {
                            track.AdditionalFields.Remove("description");
                        }
                        if (track.AdditionalFields.ContainsKey("DESCRIPTION"))
                        {
                            track.AdditionalFields.Remove("DESCRIPTION");
                        }
                        if (track.AdditionalFields.ContainsKey("purl"))
                        {
                            track.AdditionalFields.Remove("purl");
                        }
                        if (track.AdditionalFields.ContainsKey("PURL"))
                        {
                            track.AdditionalFields.Remove("PURL");
                        }
                        if (track.AdditionalFields.ContainsKey("synopsis"))
                        {
                            track.AdditionalFields.Remove("synopsis");
                        }
                        if (track.AdditionalFields.ContainsKey("SYNOPSIS"))
                        {
                            track.AdditionalFields.Remove("SYNOPSIS");
                        }
                        if (track.AdditionalFields.ContainsKey("url"))
                        {
                            track.AdditionalFields.Remove("url");
                        }
                        if (track.AdditionalFields.ContainsKey("URL"))
                        {
                            track.AdditionalFields.Remove("URL");
                        }
                        await track.SaveAsync();
                    }
                }
            }
            catch { }
        }
        ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, ReadOnlyMemory<char>.Empty));
        Completed?.Invoke(this, new DownloadCompletedEventArgs(Id, Status, FilePath, Log.AsMemory(), true));
        if (_process is not null)
        {
            _process.Exited -= Process_Exited;
            _process.ErrorDataReceived -= Process_OutputDataReceived;
            _process.OutputDataReceived -= Process_OutputDataReceived;
            _process.Dispose();
            _process = null;
        }
    }

    private async void Process_OutputDataReceived(object? sender, DataReceivedEventArgs e)
    {
        if (_progressSkipCounter > 0 || e.Data is null || string.IsNullOrEmpty(e.Data) || string.IsNullOrWhiteSpace(e.Data) || e.Data.StartsWith(" ***", StringComparison.Ordinal))
        {
            if (_progressSkipCounter > 0)
            {
                _progressSkipCounter--;
            }
            else if (e.Data?.StartsWith(" ***", StringComparison.Ordinal) ?? false)
            {
                _progressSkipCounter = 4;
            }
            return;
        }
        _logBuilder.AppendLine(e.Data);
        try
        {
            if (e.Data.StartsWith("[Parabolic] Progress", StringComparison.Ordinal))
            {
                var fields = e.Data.Split(';');
                if (fields.Length != 7 || fields[1] == "NA")
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), double.NaN, 0.0, 0));
                }
                else if (fields[1] == "finished" || fields[1] == "processing")
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), double.NaN, 0.0, 0));
                }
                else
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id,
                        e.Data.AsMemory(),
                        (fields[2] != "NA" ? double.Parse(fields[2], NumberStyles.Any, CultureInfo.InvariantCulture) : 0.0) / (fields[3] != "NA" ? double.Parse(fields[3], NumberStyles.Any, CultureInfo.InvariantCulture) : (fields[4] != "NA" ? double.Parse(fields[4], NumberStyles.Any, CultureInfo.InvariantCulture) : 1.0)),
                        fields[5] != "NA" ? double.Parse(fields[5], NumberStyles.Any, CultureInfo.InvariantCulture) : 0.0,
                        fields[6] == "NA" || fields[6] == "Unknown" ? -1 : Convert.ToInt32(double.Parse(fields[6], NumberStyles.Any, CultureInfo.InvariantCulture))));
                }
            }
            else if (e.Data.StartsWith("[#", StringComparison.Ordinal))
            {
                var line = e.Data;
                if (OperatingSystem.IsWindows())
                {
                    var index = line.LastIndexOf('\r');
                    while (index >= 0)
                    {
                        var candidate = line[(index + 1)..];
                        if (candidate.TryParseAriaProgressLine(out var progress, out var speed, out var eta))
                        {
                            ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), progress, speed, eta));
                            return;
                        }
                        line = line[..index];
                        index = line.LastIndexOf('\r');
                    }
                }
                if (!line.TryParseAriaProgressLine(out var finalProgress, out var finalSpeed, out var finalEta))
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), double.NaN, 0.0, 0));
                    return;
                }
                ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), finalProgress, finalSpeed, finalEta));
            }
            else if (e.Data.StartsWith("[download] Sleeping", StringComparison.Ordinal))
            {
                var seconds = 0.0;
                var sleepingPrefixLength = "[download] Sleeping ".Length;
                var secondsEnd = e.Data.IndexOf(" second", sleepingPrefixLength, StringComparison.Ordinal);
                if (secondsEnd == -1 || !double.TryParse(e.Data.Substring(sleepingPrefixLength, secondsEnd - sleepingPrefixLength), NumberStyles.Any, CultureInfo.InvariantCulture, out seconds))
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), double.NegativeInfinity, 0.0, 0));
                }
                else
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), double.NegativeInfinity, seconds, 0));
                    while (seconds >= 1)
                    {
                        if (Status == DownloadStatus.Paused)
                        {
                            return;
                        }
                        ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, ReadOnlyMemory<char>.Empty, double.NegativeInfinity, Math.Floor(seconds--), 0));
                        await Task.Delay(1000);
                    }
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, ReadOnlyMemory<char>.Empty, double.NaN, 0.0, 0));
                }
            }
            else
            {
                ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), double.NaN, 0.0, 0));
            }
        }
        catch
        {
            ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), double.NaN, 0.0, 0));
        }
    }
}
