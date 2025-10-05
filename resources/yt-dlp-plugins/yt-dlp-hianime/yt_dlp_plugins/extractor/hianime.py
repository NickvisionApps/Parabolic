__version__ = "3.0.0"

import sys
import os

sys.path.append(os.path.dirname(os.path.abspath(__file__)))

import re
from yt_dlp.extractor.common import InfoExtractor
from yt_dlp.utils import ExtractorError, clean_html, get_element_by_class
from megacloud import Megacloud

class HiAnimeIE(InfoExtractor):
    _VALID_URL = r'https?://hianime(?:z)?\.(?:to|is|nz|bz|pe|cx|gs|do)/(?:watch/)?(?P<slug>[^/?]+)(?:-\d+)?-(?P<playlist_id>\d+)(?:\?ep=(?P<episode_id>\d+))?$'

    _TESTS = [
        {
            'url': 'https://hianimez.to/demon-slayer-kimetsu-no-yaiba-hashira-training-arc-19107',
            'info_dict': {
                'id': '19107',
                'title': 'Demon Slayer: Kimetsu no Yaiba Hashira Training Arc',
            },
            'playlist_count': 8,
        },
        {
            'url': 'https://hianimez.to/watch/demon-slayer-kimetsu-no-yaiba-hashira-training-arc-19107?ep=124260',
            'info_dict': {
                'id': '124260',
                'title': 'To Defeat Muzan Kibutsuji',
                'ext': 'mp4',
                'series': 'Demon Slayer: Kimetsu no Yaiba Hashira Training Arc',
                'series_id': '19107',
                'episode': 'To Defeat Muzan Kibutsuji',
                'episode_number': 1,
                'episode_id': '124260',
            },
        },
        {
            'url': 'https://hianimez.to/the-eminence-in-shadow-17473',
            'info_dict': {
                'id': '17473',
                'title': 'The Eminence in Shadow',
            },
            'playlist_count': 20,
        },
        {
            'url': 'https://hianimez.to/watch/the-eminence-in-shadow-17473?ep=94440',
            'info_dict': {
                'id': '94440',
                'title': 'The Hated Classmate',
                'ext': 'mp4',
                'series': 'The Eminence in Shadow',
                'series_id': '17473',
                'episode': 'The Hated Classmate',
                'episode_number': 1,
                'episode_id': '94440',
            },
        },
    ]

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.anime_title = None
        self.episode_list = {}
        self.language = {
            'sub': 'ja',
            'dub': 'en',
            'raw': 'ja'
        }
        self.language_codes = {
            'Arabic': 'ar',
            'English Dubbed': 'en-IN',
            'English Subbed': 'en',
            'French - Francais(France)': 'fr',
            'German - Deutsch': 'de',
            'Italian - Italiano': 'it',
            'Portuguese - Portugues(Brasil)': 'pt',
            'Russian': 'ru',
            'Spanish - Espanol': 'es',
            'Spanish - Espanol(Espana)': 'es',
        }

    def _real_extract(self, url):
        mobj = self._match_valid_url(url)
        playlist_id = mobj.group('playlist_id')
        episode_id = mobj.group('episode_id')
        slug = mobj.group('slug')
        self.base_url = re.match(r'https?://[^/]+', url).group(0)

        if episode_id:
            return self._extract_episode(slug, playlist_id, episode_id)
        elif playlist_id:
            return self._extract_playlist(slug, playlist_id)
        else:
            raise ExtractorError('Unsupported URL format')

    # ========== Playlist and Episode Extraction ========== #

    # ========== Playlist Extraction ========== #

    def _extract_playlist(self, slug, playlist_id):
        anime_title = self._get_anime_title(slug, playlist_id)
        playlist_url = f'{self.base_url}/ajax/v2/episode/list/{playlist_id}'
        playlist_data = self._download_json(playlist_url, playlist_id, note='Fetching Episode List')
        episodes = self._get_elements_by_tag_and_attrib(
            playlist_data['html'], tag='a', attribute='class', value='ep-item'
        )

        entries = []
        for episode in episodes:
            html = episode.group(0)
            title = re.search(r'title="([^"]+)"', html)
            number = re.search(r'data-number="([^"]+)"', html)
            data_id = re.search(r'data-id="([^"]+)"', html)
            href = re.search(r'href="([^"]+)"', html)

            ep_id = data_id.group(1) if data_id else None
            ep_title = clean_html(title.group(1)) if title else None
            ep_number = int(number.group(1)) if number else None
            ep_url = f'{self.base_url}{href.group(1)}' if href else None

            self.episode_list[ep_id] = {
                'title': ep_title,
                'number': ep_number,
                'url': ep_url,
            }

            entries.append(self.url_result(
                ep_url,
                ie=self.ie_key(),
                video_id=ep_id,
                video_title=ep_title,
            ))

        return self.playlist_result(entries, playlist_id, anime_title)
    
    # ========== Episode Extraction ========== #

    def _extract_episode(self, slug, playlist_id, episode_id):
        anime_title = self._get_anime_title(slug, playlist_id)

        if episode_id not in self.episode_list:
            self._extract_playlist(slug, playlist_id)

        episode_data = self.episode_list.get(episode_id)

        if not episode_data:
            raise ExtractorError(f'Episode data for episode_id {episode_id} not found')

        servers_url = f'{self.base_url}/ajax/v2/episode/servers?episodeId={episode_id}'
        servers_data = self._download_json(servers_url, episode_id, note='Fetching Server IDs')

        formats = []
        subtitles = {}

        for server_type in ['sub', 'dub', 'raw']:
            # 1. Initial element fetching
            server_items_from_func = self._get_elements_by_tag_and_attrib(
                servers_data['html'], tag='div', attribute='data-type', value=server_type, escape_value=False
            )
            
            # 2. Stricter filtering
            server_items_filtered = [s for s in server_items_from_func if f'data-type="{server_type}"' in s.group(0)]
            
            # 3. Extracting the server ID
            target_link_text = "HD-1"
            server_id = next(
                (
                    # This part extracts the data-id if all conditions are met
                    re.search(r'data-id="([^"]+)"', s.group(0)).group(1)
                    
                    # Iterate through the server items already filtered by server_type
                    for s in server_items_filtered 
                    
                    # Add a condition to check for the specific link text
                    # This regex looks for "> HD-1 </a>", allowing for optional whitespace
                    if re.search(rf'>\s*{re.escape(target_link_text)}\s*</a>', s.group(0))
                    # And ensure the data-id attribute exists (good practice before .group(1))
                    and re.search(r'data-id="([^"]+)"', s.group(0))
                ),
                None  # Default value if no such item is found
            )
            if not server_id:
                continue

            sources_url = f'{self.base_url}/ajax/v2/episode/sources?id={server_id}'
            sources_data = self._download_json(sources_url, episode_id, note=f'Getting {server_type.upper()} Episode Information')
            embed_url = sources_data.get('link')
            if not embed_url:
                continue

            scraper=Megacloud(embed_url)
            data=scraper.extract()
            
            for source in data.get('sources', []):
                file_url = source.get('file')
                if not (file_url and file_url.endswith('.m3u8')):
                    continue

                extracted_formats = self._extract_custom_m3u8_formats(
                    file_url,
                    episode_id,
                    headers={"Referer": "https://megacloud.blog/"},
                    server_type=server_type
                )
                formats.extend(extracted_formats)

            for track in data.get('tracks', []):
                if track.get('kind') != 'captions':
                    continue

                file_url = track.get('file')
                label = track.get('label')

                if label == 'English':
                    label += f' {server_type.capitalize()}bed'

                lang_code = self.language_codes.get(label, label)

                if file_url:
                    subtitles.setdefault(lang_code, []).append({
                        'name': label,
                        'url': file_url,
                    })
        return {
            'id': episode_id,
            'title': episode_data['title'],
            'formats': formats,
            'subtitles': subtitles,
            'series': anime_title,
            'series_id': playlist_id,
            'episode': episode_data['title'],
            'episode_number': episode_data['number'],
            'episode_id': episode_id,
        }

    # ========== Helpers ========== #

    def _extract_custom_m3u8_formats(self, m3u8_url, episode_id, headers, server_type=None):
        formats = self._extract_m3u8_formats(
            m3u8_url, episode_id, 'mp4', entry_protocol='m3u8_native',
            note='Downloading M3U8 Information', headers=headers
        )
        for f in formats:
            height = f.get('height')
            f['format_id'] = f'{server_type}_{height}p'
            f['language'] = self.language[server_type]
            f['http_headers'] = headers
        return formats

    def _get_anime_title(self, slug, playlist_id):
        if self.anime_title:
            return self.anime_title
        webpage = self._download_webpage(
            f'{self.base_url}/{slug}-{playlist_id}',
            playlist_id,
            note='Fetching Anime Title'
        )
        self.anime_title = get_element_by_class('film-name dynamic-name', webpage)
        return self.anime_title

    def _get_elements_by_tag_and_attrib(self, html, tag=None, attribute=None, value=None, escape_value=True):
        tag = tag or r'[a-zA-Z0-9:._-]+'
        if attribute:
            attribute = rf'\s+{re.escape(attribute)}'
        if value:
            value = re.escape(value) if escape_value else value
            value = f'=[\'"]?(?P<value>.*?{value}.*?)[\'"]?'

        return list(re.finditer(rf'''(?xs)
            <{tag}
            (?:\s+[a-zA-Z0-9:._-]+(?:=[a-zA-Z0-9:._-]*|="[^"]*"|='[^']*'|))*?
            {attribute}{value}
            (?:\s+[a-zA-Z0-9:._-]+(?:=[a-zA-Z0-9:._-]*|="[^"]*"|='[^']*'|))*?
            \s*>
            (?P<content>.*?)
            </{tag}>
        ''', html))