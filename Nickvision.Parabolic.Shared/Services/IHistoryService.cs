using Nickvision.Desktop;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public interface IHistoryService : IService
{
    HistoryLength Length { get; set; }

    public Task<bool> AddAsync(HistoricDownload download);
    public Task<bool> AddAsync(IEnumerable<HistoricDownload> downloads);
    public Task<bool> ClearAsync();
    public Task<IEnumerable<HistoricDownload>> GetAllAsync();
    public Task<bool> RemoveAsync(HistoricDownload download);
    public Task<bool> RemoveAsync(Uri uri);
    public Task<bool> UpdateAsync(HistoricDownload download);


}
