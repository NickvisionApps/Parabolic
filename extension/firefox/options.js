// Load saved settings when options opens
document.addEventListener('DOMContentLoaded', () => {
  browser.storage.sync.get(['trimPlaylist']).then((result) => {
    document.getElementById('trimPlaylist').checked = (result && result.trimPlaylist) || false;
  }).catch((error) => {
    console.error('Storage error:', error);
  });
});

// Save setting when checkbox is toggled
document.getElementById('trimPlaylist').addEventListener('change', (e) => {
  browser.storage.sync.set({ trimPlaylist: e.target.checked });
});