// Import shared utilities
importScripts('utils.js');

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

// Function to create the context menu item
function createContextMenu() {
  chrome.contextMenus.create({
    id: "openParabolicLink",
    title: "Open link in Parabolic",
    contexts: ["link", "page"]
  });
}

// Function to remove the context menu item
function removeContextMenu() {
  chrome.contextMenus.remove("openParabolicLink");
}

// Initialize context menu based on stored settings
chrome.runtime.onInstalled.addListener(() => {
  chrome.storage.sync.get('showContextMenu', (result) => {
    if (result.showContextMenu !== false) {
      createContextMenu();
    }
  });
});

// Listen for changes in storage to update the context menu
chrome.storage.onChanged.addListener((changes, namespace) => {
  if (namespace === 'sync' && changes.showContextMenu) {
    if (changes.showContextMenu.newValue) {
      createContextMenu();
    } else {
      removeContextMenu();
    }
  }
});

// Listener for context menu: Open link in Parabolic clicks
chrome.contextMenus.onClicked.addListener((info, tab) => {
  if (info.menuItemId === "openParabolicLink") {
    // If the right click was on a link, use the link's URL.
    // Otherwise, use the current tab's URL.
    const urlToOpen = info.linkUrl || info.pageUrl;
    openParabolicUrl(urlToOpen);
  }
});

// Listener for action (Extension icon) clicks
chrome.action.onClicked.addListener((tab) => {
  // Use the current tab's URL for the action button
  openParabolicUrl(tab.url);
});

// Listener for Keyboard shorcut command(Alt+P, Alt+O)
chrome.commands.onCommand.addListener((command) => {
  if (command === "open-parabolic-current-tab") {
    chrome.tabs.query({ active: true, currentWindow: true }, (tabs) => {
      if (tabs.length > 0) {
        openParabolicUrl(tabs[0].url);
      }
    });
  };
  if (command === "open-parabolic-options") {
    chrome.runtime.openOptionsPage();
  }
});