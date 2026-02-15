using Nickvision.Desktop;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public interface IFileMetadataService : IService
{
    Task<bool> RemoveSourceDataAsync(string filePath);
}
