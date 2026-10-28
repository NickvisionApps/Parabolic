"""Extract the selected Facebook reel's browser-native MP4, never a recommendation."""
import json
import re
import subprocess
from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import parse_qs, urlparse

from yt_dlp.extractor.common import InfoExtractor
from yt_dlp.utils import ExtractorError


class _Scripts(HTMLParser):
    def __init__(self):
        super().__init__(convert_charrefs=False)
        self.scripts = []
        self.current = None
        self.canonical = None
        self.title = None

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        if tag == 'script':
            self.current = []
        elif tag == 'link' and attrs.get('rel') == 'canonical':
            self.canonical = attrs.get('href')
        elif tag == 'meta' and attrs.get('property') == 'og:title':
            self.title = attrs.get('content')
        elif tag == 'meta' and attrs.get('property') == 'og:url':
            self.canonical = self.canonical or attrs.get('content')

    def handle_data(self, data):
        if self.current is not None:
            self.current.append(data)

    def handle_endtag(self, tag):
        if tag == 'script' and self.current is not None:
            self.scripts.append(''.join(self.current))
            self.current = None


def video_id(url):
    parsed = urlparse(url)
    match = re.search(r'/(?:reel|videos)/(\d+)', parsed.path)
    if match:
        return match[1]
    return parse_qs(parsed.query).get('v', [None])[0]


def selected_media(webpage, resolved_url):
    parser = _Scripts()
    parser.feed(webpage)
    target = video_id(resolved_url) or video_id(parser.canonical or '')
    if not target:
        return None
    stack = []
    for script in parser.scripts:
        try:
            stack.append(json.loads(script))
        except (ValueError, TypeError):
            pass
    while stack:
        obj = stack.pop()
        if isinstance(obj, list):
            stack.extend(obj)
        elif isinstance(obj, dict):
            if str(obj.get('id')) == target and (obj.get('browser_native_hd_url') or obj.get('browser_native_sd_url')):
                return {'id': target, 'hd': obj.get('browser_native_hd_url'),
                        'sd': obj.get('browser_native_sd_url'), 'title': parser.title,
                        'url': resolved_url if video_id(resolved_url) else parser.canonical}
            stack.extend(v for v in obj.values() if isinstance(v, (dict, list)))
    return None


def allowed_media(url):
    if not isinstance(url, str):
        return False
    parsed = urlparse(url)
    host = (parsed.hostname or '').lower()
    return parsed.scheme == 'https' and host.endswith('.fbcdn.net') and not parsed.username


class FacebookBrowserIE(InfoExtractor):
    IE_NAME = 'facebook:browser'
    _VALID_URL = r'https?://(?:www\.|m\.|web\.)?facebook\.com/(?:share/r/(?P<share>[A-Za-z0-9]+)|reel/(?P<id>\d+))/?(?:[?#].*)?$'

    def _real_extract(self, url):
        media = None
        reader = self._configuration_arg('page_reader', [None])[0]
        if reader and Path(reader).is_file():
            self.to_screen('Reading Facebook reel in WebKit')
            try:
                result = subprocess.run([reader, url], capture_output=True, text=True, timeout=50)
                if result.returncode == 0:
                    media = json.loads(result.stdout)
                else:
                    self.report_warning('Browser reader: ' + result.stderr.strip()[:300])
            except (OSError, subprocess.TimeoutExpired, ValueError) as error:
                self.report_warning('Browser reader failed: ' + str(error))
        if not media:
            webpage, response = self._download_webpage_handle(url, 'facebook-reel')
            media = selected_media(webpage, response.url)
        if not media:
            raise ExtractorError('The selected public reel could not be read. Check the link and visibility; login-required reels are not supported by this reader.', expected=True)
        # A renderer must also return the selected ID, not the first video in its feed.
        target = video_id(media.get('url', '')) or video_id(url)
        if not target or str(media.get('id')) != target:
            raise ExtractorError('Facebook returned a different video ID', expected=True)
        formats = []
        for key, quality in [('sd', 0), ('hd', 1)]:
            direct = media.get(key)
            if allowed_media(direct) and direct not in [f['url'] for f in formats]:
                formats.append({'format_id': 'browser-' + key, 'format_note': key.upper(),
                                'url': direct, 'ext': 'mp4', 'quality': quality,
                                'vcodec': 'h264', 'acodec': 'aac'})
        if not formats:
            raise ExtractorError('No valid Facebook MP4 address was returned', expected=True)
        description = media.get('title') or ''
        title = description.splitlines()[0][:180] if description else 'Facebook ' + target
        return {'id': target, 'title': title, 'description': description,
                'webpage_url': media.get('url') or url, 'formats': formats,
                'http_headers': {'Referer': 'https://www.facebook.com/',
                                 'User-Agent': 'Mozilla/5.0'}}
