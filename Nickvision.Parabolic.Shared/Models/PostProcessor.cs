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

public static class PostProcessorExtensions
{
    extension(PostProcessor pp)
    {
        public string ToYtdlpString() => pp switch
        {
            PostProcessor.Merger => "Merger",
            PostProcessor.ModifyChapters => "ModifyChapters",
            PostProcessor.SplitChapters => "SplitChapters",
            PostProcessor.ExtractAudio => "ExtractAudio",
            PostProcessor.VideoRemuxer => "VideoRemuxer",
            PostProcessor.VideoConverter => "VideoConverter",
            PostProcessor.Metadata => "Metadata",
            PostProcessor.EmbedSubtitle => "EmbedSubtitle",
            PostProcessor.EmbedThumbnail => "EmbedThumbnail",
            PostProcessor.SubtitlesConverter => "SubtitlesConverter",
            PostProcessor.ThumbnailsConverter => "ThumbnailsConverter",
            PostProcessor.FixupStretched => "FixupStretched",
            PostProcessor.FixupM4a => "FixupM4a",
            PostProcessor.FixupM3u8 => "FixupM3u8",
            PostProcessor.FixupTimestamp => "FixupTimestamp",
            PostProcessor.FixupDuration => "FixupDuration",
            _ => string.Empty
        };
    }
}
