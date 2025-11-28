// Function to format the URL and open the scheme link
function openParabolicUrl(url) {
  chrome.storage.sync.get(['trimPlaylist'], (result) => {
    if (chrome.runtime.lastError) {
      console.error('Storage error:', chrome.runtime.lastError);
    }
    let processedUrl = url;
    if (result && result.trimPlaylist) {
      processedUrl = trimPlaylistFromUrl(url);
    }
    // Remove "https://" or "http://" from the URL
    let formattedUrl = processedUrl.replace(/^https?:\/\//, '');
    // Construct the final scheme URL
    let schemeUrl = `parabolic://${formattedUrl}`;

    // Use the chrome.tabs API to open the URL scheme
    chrome.tabs.update({ url: schemeUrl });
  });
}

// Load saved settings when popup opens
document.addEventListener('DOMContentLoaded', () => {
  chrome.storage.sync.get(['trimPlaylist'], (result) => {
    if (chrome.runtime.lastError) {
      console.error('Storage error:', chrome.runtime.lastError);
      return;
    }
    document.getElementById('trimPlaylist').checked = (result && result.trimPlaylist) || false;
  });
});

// Save setting when checkbox is toggled
document.getElementById('trimPlaylist').addEventListener('change', (e) => {
  chrome.storage.sync.set({ trimPlaylist: e.target.checked });
});

// Send current page button handler
document.getElementById('sendBtn').addEventListener('click', () => {
  chrome.tabs.query({ active: true, currentWindow: true }, (tabs) => {
    if (tabs.length > 0) {
      openParabolicUrl(tabs[0].url);
    }
  });
});
