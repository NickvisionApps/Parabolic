#!/usr/bin/env python3
from yt_dlp.postprocessor.ffmpeg import FFmpegMetadataPP

class TCMetadataPP(FFmpegMetadataPP):
    def run(self, info):
        try:
            success = super().run(info)
            return success
        except:
            self.to_screen('WARNING: Failed to embed metadata')
            return [], info
