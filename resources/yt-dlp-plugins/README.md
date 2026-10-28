# yt-dlp-plugins

Adapted from:

- https://github.com/bindestriche/srt_fix

## Facebook browser-native downloads

The `facebook_browser` extractor handles public Facebook reel and shared-reel
URLs. It matches the selected video ID before reading `browser_native_hd_url`
and `browser_native_sd_url`, so recommended videos embedded in the same page
cannot be selected accidentally. Available progressive MP4 streams are exposed
as `browser-hd` and `browser-sd` to the normal yt-dlp download pipeline.

On macOS, the packaging script builds a WebKit page reader. Parabolic passes its
path to the extractor during both discovery and downloading. The reader uses a
nonpersistent session, does not import browser cookies, and exits after at most
40 seconds. Other platforms use page HTML directly. Login-required or
inaccessible reels produce an extraction error; this reader does not provide a
login flow or guarantee the highest DASH resolution.

Run the offline regression tests with Python and yt-dlp installed:

```sh
python -m unittest discover -s tests -p 'test_facebook_browser.py' -v
```
