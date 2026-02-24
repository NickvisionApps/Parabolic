using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public interface IHistoryService
{
    bool SortNewest { get; set; }
    HistoryLength Length { get; set; }

    Task<bool> AddAsync(HistoricDownload download);
    Task<bool> AddAsync(IReadOnlyList<HistoricDownload> downloads);
    Task<bool> ClearAsync();
    Task<IReadOnlyList<HistoricDownload>> GetAllAsync();
    Task<bool> RemoveAsync(HistoricDownload download);
    Task<bool> RemoveAsync(Uri uri);
    Task<bool> UpdateAsync(HistoricDownload download);
}
