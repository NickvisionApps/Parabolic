// Load saved settings when options opens
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

