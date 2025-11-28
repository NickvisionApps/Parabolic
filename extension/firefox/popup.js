// Function to format the URL and open the scheme link
function openParabolicUrl(url) {
  browser.storage.sync.get(['trimPlaylist']).then((result) => {
    let processedUrl = url;
    if (result.trimPlaylist) {
      processedUrl = trimPlaylistFromUrl(url);
    }
    // Remove "https://" or "http://" from the URL
    let formattedUrl = processedUrl.replace(/^https?:\/\//, '');
    // Construct the final scheme URL
    let schemeUrl = `parabolic://${formattedUrl}`;

    // Use the browser.tabs API to open the URL scheme
    browser.tabs.update({ url: schemeUrl });
  });
}

// Load saved settings when popup opens
document.addEventListener('DOMContentLoaded', () => {
  browser.storage.sync.get(['trimPlaylist']).then((result) => {
    document.getElementById('trimPlaylist').checked = result.trimPlaylist || false;
  });
});

// Save setting when checkbox is toggled
document.getElementById('trimPlaylist').addEventListener('change', (e) => {
  browser.storage.sync.set({ trimPlaylist: e.target.checked });
});

// Send current page button handler
document.getElementById('sendBtn').addEventListener('click', () => {
  browser.tabs.query({ active: true, currentWindow: true }).then((tabs) => {
    if (tabs.length > 0) {
      openParabolicUrl(tabs[0].url);
    }
  });
});
