// Load saved settings when options opens
document.addEventListener('DOMContentLoaded', () => {
  browser.storage.sync.get(['trimPlaylist', 'showContextMenu']).then((result) => {
    document.getElementById('trimPlaylist').checked = (result && result.trimPlaylist) || false;
    document.getElementById('showContextMenu').checked = (result && (typeof result.showContextMenu !== 'undefined' ? result.showContextMenu : true));
  }).catch((error) => {
    console.error('Storage error:', error);
  });
});

// Save setting when checkbox is toggled
document.getElementById('trimPlaylist').addEventListener('change', (e) => {
  browser.storage.sync.set({ trimPlaylist: e.target.checked });
});

// Save/show context menu setting
document.getElementById('showContextMenu').addEventListener('change', (e) => {
  browser.storage.sync.set({ showContextMenu: e.target.checked });
});