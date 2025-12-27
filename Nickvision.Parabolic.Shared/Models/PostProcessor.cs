namespace Nickvision.Parabolic.Shared.Models;

public enum PostProcessor
{
    None,
    Merger,
    ModifyChapters,
    SplitChapters,
    ExtractAudio,
    VideoRemuxer,
    VideoConverter,
    Metadata,
    EmbedSubtitle,
    EmbedThumbnail,
    SubtitlesConverter,
    ThumbnailsConverter,
    FixupStretched,
    FixupM4a,
    FixupM3u8,
    FixupTimestamp,
    FixupDuration
}
