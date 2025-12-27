using System;

namespace Nickvision.Parabolic.Shared.Models;

public class HistoricDownload
{
    public Uri Url { get; }
    public string Title { get; set; }
    public string Path { get; set; }
    public DateTime DownloadedOn { get; set; }

    public HistoricDownload(Uri url)
    {
        Url = url;
        Title = string.Empty;
        Path = string.Empty;
        DownloadedOn = DateTime.Now;
    }

    public int CompareTo(HistoricDownload? other)
    {
        if (other is null)
        {
            return 1;
        }
        return Url.ToString().CompareTo(other.Url.ToString());
    }

    public override bool Equals(object? obj) => obj is HistoricDownload other && Equals(other);

    public bool Equals(HistoricDownload? other) => other is not null && Url == other.Url;

    public override int GetHashCode() => Url.GetHashCode();

    public static bool operator >(HistoricDownload left, HistoricDownload right) => left.CompareTo(right) > 0;

    public static bool operator <(HistoricDownload left, HistoricDownload right) => left.CompareTo(right) < 0;

    public static bool operator >=(HistoricDownload left, HistoricDownload right) => left.CompareTo(right) >= 0;

    public static bool operator <=(HistoricDownload left, HistoricDownload right) => left.CompareTo(right) <= 0;

    public static bool operator ==(HistoricDownload left, HistoricDownload right) => left.Equals(right);

    public static bool operator !=(HistoricDownload left, HistoricDownload right) => !left.Equals(right);
}
