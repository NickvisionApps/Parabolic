// Shared utility functions for Parabolic Extension

// Function to trim playlist parameters from YouTube URLs
function trimPlaylistFromUrl(url) {
  try {
    const urlObj = new URL(url);
    // Check if it's a YouTube URL
    if (urlObj.hostname.includes('youtube.com') || urlObj.hostname.includes('youtu.be')) {
      // Remove list parameter and related parameters
      urlObj.searchParams.delete('list');
      urlObj.searchParams.delete('index');
      return urlObj.toString();
    }
  } catch (e) {
    // If URL parsing fails, return original
  }
  return url;
}
