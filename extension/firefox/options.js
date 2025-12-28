// Load saved settings when options opens
document.addEventListener('DOMContentLoaded', () => {
  browser.storage.sync.get(['trimPlaylist', 'showContextMenu']).then((result) => {
    if (browser.runtime.lastError) {
      console.error('Storage error:', browser.runtime.lastError);
      return;
    }
    document.getElementById('trimPlaylist').checked = (result && result.trimPlaylist) || false;
    document.getElementById('contextMenuToggle').checked = (result && (typeof result.showContextMenu !== 'undefined' ? result.showContextMenu : true));
  });
});

// Save/show Trim Playlist setting
document.getElementById('trimPlaylist').addEventListener('change', (e) => {
  browser.storage.sync.set({ trimPlaylist: e.target.checked });
});

// Save/show Context Menu setting
document.getElementById('contextMenuToggle').addEventListener('change', (e) => {
  chrome.storage.sync.set({ showContextMenu: e.target.checked });
});