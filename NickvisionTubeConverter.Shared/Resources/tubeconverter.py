#!/usr/bin/env python3
from yt_dlp.postprocessor.ffmpeg import FFmpegMetadataPP, FFmpegSubtitlesConvertorPP, FFmpegEmbedSubtitlePP
from yt_dlp.postprocessor.embedthumbnail import EmbedThumbnailPP
import os, re

class TCMetadataPP(FFmpegMetadataPP):
    def run(self, info):
        try:
            success = super().run(info)
            return success
        except Exception as e:
            self.to_screen(e)
            self.to_screen('WARNING: Failed to embed metadata')
            return [], info

class TCSubtitlesConvertorPP(FFmpegSubtitlesConvertorPP):
    def run(self, info):
        # Remove styling from VTT files before processing
        # https://trac.ffmpeg.org/ticket/8684
        subs = info.get('requested_subtitles')
        if subs is not None:
            for _, sub in subs.items():
                if os.path.exists(sub.get('filepath', '')):
                    with open(sub.get('filepath', ''), 'r') as f:
                        data = f.read()
                    data = re.sub(r'^STYLE\n(::cue.*\{\n*[^}]*\}\n+)+', '', data, flags=re.MULTILINE)
                    with open(sub.get('filepath', ''), 'w') as f:
                        f.write(data)
        return super().run(info)


class TCEmbedSubtitlePP(FFmpegEmbedSubtitlePP):
    def run(self, info):
        try:
            success = super().run(info)
            return success
        except Exception as e:
          self.to_screen(e)
          self.to_screen('WARNING: Failed to embed subtitles')
          return [], info

class TCEmbedThumbnailPP(EmbedThumbnailPP):
    def run(self, info):
        try:
            success = super().run(info)
            return success
        except Exception as e:
            self.to_screen(e)
            self.to_screen('WARNING: Failed to embed thumbnail')
            idx = next((-i for i, t in enumerate(info['thumbnails'][::-1], 1) if t.get('filepath')), None)
            thumbnail_filename = info['thumbnails'][idx]['filepath']
            self._delete_downloaded_files(thumbnail_filename, info=info)
            return [], info

