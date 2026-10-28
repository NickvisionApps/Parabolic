import importlib.util
import json
from pathlib import Path
from types import SimpleNamespace
import unittest
from unittest.mock import patch

from yt_dlp import YoutubeDL
from yt_dlp.utils import ExtractorError

path = Path(__file__).resolve().parents[1] / 'resources/yt-dlp-plugins/facebook_browser/yt_dlp_plugins/extractor/facebook_browser.py'
spec = importlib.util.spec_from_file_location('facebook_browser_test', path)
m = importlib.util.module_from_spec(spec)
spec.loader.exec_module(m)

def page(objects, canonical='https://www.facebook.com/reel/123'):
    return '<link rel="canonical" href="' + canonical + '"><meta property="og:title" content="Exercise &amp; control"><script type="application/json">' + json.dumps(objects) + '</script>'

class ExtractionTests(unittest.TestCase):
    def test_target_after_recommendation_and_escaped_url(self):
        html = page({'data': [{'id': '999', 'browser_native_hd_url': 'https://video.xx.fbcdn.net/wrong.mp4'},
                              {'id': '123', 'browser_native_hd_url': 'https://video.xx.fbcdn.net/right.mp4?a=1&b=2'}]})
        result = m.selected_media(html, 'https://www.facebook.com/reel/123')
        self.assertEqual(result['hd'], 'https://video.xx.fbcdn.net/right.mp4?a=1&b=2')
        self.assertEqual(result['title'], 'Exercise & control')

    def test_share_uses_canonical_id(self):
        result = m.selected_media(page({'id': '123', 'browser_native_sd_url': 'https://video.xx.fbcdn.net/sd.mp4'}), 'https://www.facebook.com/share/r/token/')
        self.assertEqual(result['id'], '123')
        self.assertEqual(result['url'], 'https://www.facebook.com/reel/123')

    def test_missing_target_never_uses_recommendation(self):
        self.assertIsNone(m.selected_media(page({'id': '999', 'browser_native_hd_url': 'https://video.xx.fbcdn.net/wrong.mp4'}), 'https://www.facebook.com/reel/123'))

    def test_safe_media_hosts(self):
        for bad in ['file:///tmp/video', 'http://video.xx.fbcdn.net/v.mp4', 'https://fbcdn.net.evil.test/v.mp4', 'https://user@video.xx.fbcdn.net/v.mp4']:
            self.assertFalse(m.allowed_media(bad))
        self.assertTrue(m.allowed_media('https://video.xx.fbcdn.net/v.mp4'))

    def test_formats_and_share_resolution(self):
        ie = m.FacebookBrowserIE(YoutubeDL({'quiet': True}))
        html = page({'id': '123', 'browser_native_hd_url': 'https://video.xx.fbcdn.net/hd.mp4',
                     'browser_native_sd_url': 'https://video.xx.fbcdn.net/sd.mp4'})
        with patch.object(ie, '_download_webpage_handle', return_value=(html, SimpleNamespace(url='https://www.facebook.com/reel/123'))):
            result = ie._real_extract('https://www.facebook.com/share/r/token/')
        self.assertEqual(result['id'], '123')
        self.assertEqual([f['format_id'] for f in result['formats']], ['browser-sd', 'browser-hd'])
        self.assertTrue(all(f['acodec'] == 'aac' for f in result['formats']))

    def test_inaccessible_reel_reports_expected_error(self):
        ie = m.FacebookBrowserIE(YoutubeDL({'quiet': True}))
        with patch.object(ie, '_download_webpage_handle', return_value=('<html>Login required</html>', SimpleNamespace(url='https://www.facebook.com/reel/123'))):
            with self.assertRaises(ExtractorError) as error:
                ie._real_extract('https://www.facebook.com/reel/123')
        self.assertTrue(error.exception.expected)

if __name__ == '__main__':
    unittest.main()
