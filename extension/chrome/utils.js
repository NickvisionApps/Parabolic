// Shared utility functions for Parabolic Extension

// Check if a hostname is a valid YouTube domain
function isYouTubeHost(hostname) {
  // Normalize hostname to lowercase
  const host = hostname.toLowerCase();
  // Check for exact match or valid subdomain patterns
  return host === 'youtube.com' ||
         host === 'www.youtube.com' ||
         host === 'm.youtube.com' ||
         host === 'music.youtube.com' ||
         host === 'youtu.be' ||
         host === 'www.youtu.be';
}

// Function to trim playlist parameters from YouTube URLs
function trimPlaylistFromUrl(url) {
  try {
    const urlObj = new URL(url);
    // Check if it's a YouTube URL
    if (isYouTubeHost(urlObj.hostname)) {
      // Remove list parameter and related parameters
      urlObj.searchParams.delete('list');
      urlObj.searchParams.delete('index');
      return urlObj.toString();
    }
  } catch (e) {
    // If URL parsing fails (e.g., malformed URL, invalid protocol), return original URL unchanged
  }
  return url;
}
