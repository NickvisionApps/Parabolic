#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief Post-processors exposed by yt-dlp.
     */
    enum class PostProcessor
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
    };
}

#endif //POSTPROCESSOR_H
