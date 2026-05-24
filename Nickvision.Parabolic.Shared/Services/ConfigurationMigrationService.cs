using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.System;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class ConfigurationMigrationService : IHostedService
{
    private readonly ILogger<ConfigurationMigrationService> _logger;
    private readonly IConfigurationService _configurationService;
    private readonly AppInfo _appInfo;
    private readonly string _directory;

    public ConfigurationMigrationService(ILogger<ConfigurationMigrationService> logger, AppInfo appInfo, IConfigurationService configurationService)
    {
        _logger = logger;
        _configurationService = configurationService;
        _appInfo = appInfo;
        _directory = appInfo.IsPortable ? Environment.ExecutingDirectory : Path.Combine(UserDirectories.Config, _appInfo.Name);
    }

    public async Task StartAsync(CancellationToken cancellationToken)
    {
        _logger.LogInformation("Starting configuration migration...");
        if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
        {
            var oldPath = Path.Combine(UserDirectories.Home, "Library", "ApplicationSupport", _appInfo.Name);
            if (Directory.Exists(oldPath))
            {
                Directory.Move(oldPath, _directory);
                Directory.Delete(oldPath, true);
            }
        }
        var configPath = Path.Combine(_directory, "config.json");
        if (File.Exists(configPath))
        {
            _logger.LogInformation($"Migrating configuration file ({configPath})...");
            var res = await _configurationService.ImportFromJsonFileAsync(configPath);
            _logger.LogInformation($"Migrated {res} properties from configuration file ({configPath}).");
            File.Delete(configPath);
        }
        var prevPath = Path.Combine(_directory, "previous.json");
        if (File.Exists(prevPath))
        {
            _logger.LogInformation($"Migrating configuration file ({prevPath})...");
            var res = await _configurationService.ImportFromJsonFileAsync(prevPath);
            _logger.LogInformation($"Migrated {res} properties from configuration file ({prevPath}).");
            File.Delete(prevPath);
        }
        _logger.LogInformation("Finished configuration migration.");
    }

    public Task StopAsync(CancellationToken cancellationToken) => Task.CompletedTask;
}
