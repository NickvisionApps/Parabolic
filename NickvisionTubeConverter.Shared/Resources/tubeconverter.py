#!/usr/bin/env python3
from yt_dlp.postprocessor.ffmpeg import FFmpegMetadataPP
from yt_dlp.postprocessor.embedthumbnail import EmbedThumbnailPP

class TCMetadataPP(FFmpegMetadataPP):
    def run(self, info):
        try:
            success = super().run(info)
            return success
        except Exception as e:
            self.to_screen(e)
            self.to_screen('WARNING: Failed to embed metadata')
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

