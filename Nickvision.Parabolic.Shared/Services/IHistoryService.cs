using Nickvision.Desktop;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public interface IHistoryService : IService
{
    HistoryLength Length { get; set; }

    Task<bool> AddAsync(HistoricDownload download);
    Task<bool> AddAsync(IEnumerable<HistoricDownload> downloads);
    Task<bool> ClearAsync();
    Task<IEnumerable<HistoricDownload>> GetAllAsync();
    Task<bool> RemoveAsync(HistoricDownload download);
    Task<bool> RemoveAsync(Uri uri);
    Task<bool> UpdateAsync(HistoricDownload download);
}
