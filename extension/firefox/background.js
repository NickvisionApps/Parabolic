// Function to format the URL and open the scheme link
function openParabolicUrl(url) {
  browser.storage.sync.get(['trimPlaylist'], (result) => {
    if (browser.runtime.lastError) {
      console.error('Storage error:', browser.runtime.lastError);
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
    browser.tabs.update({ url: schemeUrl });
  });
}

// Function to create the context menu item
function createContextMenu() {
  browser.contextMenus.create({
    id: "openParabolicLink",
    title: "Open link in Parabolic",
    contexts: ["link", "page"]
  });
}

// Function to remove the context menu item
function removeContextMenu() {
  browser.contextMenus.remove("openParabolicLink");
}

// Initialize context menu based on stored settings
browser.runtime.onInstalled.addListener(() => {
  browser.storage.sync.get('showContextMenu', (result) => {
    if (result.showContextMenu !== false) {
      createContextMenu();
    }
  });
});

// Listen for changes in storage to update the context menu
browser.storage.onChanged.addListener((changes, namespace) => {
  if (namespace === 'sync' && changes.showContextMenu) {
    if (changes.showContextMenu.newValue) {
      createContextMenu();
    } else {
      removeContextMenu();
    }
  }
});

// Listener for context menu: Open link in Parabolic clicks
browser.contextMenus.onClicked.addListener((info, tab) => {
  if (info.menuItemId === "openParabolicLink") {
    // If the right click was on a link, use the link's URL.
    // Otherwise, use the current tab's URL.
    const urlToOpen = info.linkUrl || info.pageUrl;
    openParabolicUrl(urlToOpen);
  }
});

// Listener for action (Extension icon) clicks
browser.action.onClicked.addListener((tab) => {
  // Use the current tab's URL for the action button
  openParabolicUrl(tab.url);
});

// Listener for Keyboard shorcut command(Alt+P, Alt+O)
browser.commands.onCommand.addListener((command) => {
  if (command === "open-parabolic-current-tab") {
    browser.tabs.query({ active: true, currentWindow: true }, (tabs) => {
      if (tabs.length > 0) {
        openParabolicUrl(tabs[0].url);
      }
    });
  };
  if (command === "open-parabolic-options") {
    browser.runtime.openOptionsPage();
  }
});