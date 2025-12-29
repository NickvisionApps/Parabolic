using Nickvision.Desktop;
using Nickvision.Parabolic.Shared.Models;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public interface IRecoveryService : IService
{
    Task<bool> AddAsync(RecoverableDownload download);
    Task<bool> AddAsync(IReadOnlyList<RecoverableDownload> downloads);
    Task<bool> ClearAsync();
    public Task<IReadOnlyList<RecoverableDownload>> GetAllAsync();
    Task<bool> RemoveAsync(RecoverableDownload download);
    Task<bool> RemoveAsync(int id);
    Task<bool> RemoveAsync(IReadOnlyList<int> ids);
}
