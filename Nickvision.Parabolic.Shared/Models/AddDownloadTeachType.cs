using System;

namespace Nickvision.Parabolic.Shared.Models;

[Flags]
public enum AddDownloadTeachType
{
    None = 0,
    DownloadImmediately = 1 << 0,
    FileType = 1 << 1,
}
