// Load saved settings when options opens
document.addEventListener('DOMContentLoaded', () => {
  chrome.storage.sync.get(['trimPlaylist', 'showContextMenu'], (result) => {
    if (chrome.runtime.lastError) {
      console.error('Storage error:', chrome.runtime.lastError);
      return;
    }
    document.getElementById('trimPlaylist').checked = (result && result.trimPlaylist) || false;
    document.getElementById('contextMenuToggle').checked = (result && (typeof result.showContextMenu !== 'undefined' ? result.showContextMenu : true));
  });
});

// Save/show Trim Playlist setting
document.getElementById('trimPlaylist').addEventListener('change', (e) => {
  chrome.storage.sync.set({ trimPlaylist: e.target.checked });
});

// Save/show context menu setting
document.getElementById('contextMenuToggle').addEventListener('change', (e) => {
  chrome.storage.sync.set({ showContextMenu: e.target.checked });
});

