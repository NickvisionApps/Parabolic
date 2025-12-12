// Function to format the URL and open the scheme link
function openParabolicUrl(url) {
  browser.storage.sync.get(['trimPlaylist']).then((result) => {
    let processedUrl = url;
    if (result && result.trimPlaylist) {
      processedUrl = trimPlaylistFromUrl(url);
    }
    // Remove "https://" or "http://" from the URL
    let formattedUrl = processedUrl.replace(/^https?:\/\//, '');
    // Construct the final scheme URL
    let schemeUrl = `parabolic://${formattedUrl}`;

    // Use the browser.tabs API to open the URL scheme
    browser.tabs.update({ url: schemeUrl });
  }).catch((error) => {
    console.error('Storage error:', error);
    // On error, proceed with original URL
    let formattedUrl = url.replace(/^https?:\/\//, '');
    let schemeUrl = `parabolic://${formattedUrl}`;
    browser.tabs.update({ url: schemeUrl });
  });
}

// Creates the context menu item when the extension is installed
browser.runtime.onInstalled.addListener(() => {
  browser.storage.sync.get(['showContextMenu']).then((result) => {
    const show = result && (typeof result.showContextMenu !== 'undefined' ? result.showContextMenu : true);
    if (show) {
      try {
        browser.contextMenus.create({
          id: "openParabolicLink",
          title: "Open link in Parabolic",
          contexts: ["page", "link"] // Shows the menu on the page and on links
        });
      } catch (e) {
        // some browser builds may not return a promise; ignore synchronous throw
      }
    }
  }).catch((error) => {
    console.error('Storage error:', error);
    // fallback: create menu by default
    try {
      browser.contextMenus.create({
        id: "openParabolicLink",
        title: "Open link in Parabolic",
        contexts: ["page", "link"]
      });
    } catch (e) {
      // ignore
    }
  });
});

// React to changes in the showContextMenu setting and add/remove menu dynamically
browser.storage.onChanged.addListener((changes, area) => {
  if (area !== 'sync') return;
  if (!changes.showContextMenu) return;
  const newVal = changes.showContextMenu.newValue;
  if (newVal) {
    // remove may be synchronous or return a promise depending on platform
    try {
      const maybe = browser.contextMenus.remove('openParabolicLink');
      Promise.resolve(maybe).catch(() => {}).then(() => {
        try {
          browser.contextMenus.create({
            id: "openParabolicLink",
            title: "Open link in Parabolic",
            contexts: ["page", "link"]
          });
        } catch (e) {
          // ignore
        }
      });
    } catch (e) {
      // if synchronous remove threw, still attempt to create
      try {
        browser.contextMenus.create({
          id: "openParabolicLink",
          title: "Open link in Parabolic",
          contexts: ["page", "link"]
        });
      } catch (err) {}
    }
  } else {
    try {
      const maybe = browser.contextMenus.remove('openParabolicLink');
      Promise.resolve(maybe).catch(() => {});
    } catch (e) {
      // ignore
    }
  }
});

// Listener for action (Extension icon) clicks
browser.action.onClicked.addListener((tab) => {
  // Use the current tab's URL for the action button
  openParabolicUrl(tab.url);
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

// Listener for Keyboard shorcut command(Alt+P)
browser.commands.onCommand.addListener((command) => {
  if (command === "open-parabolic-current-tab") {
    browser.tabs.query({ active: true, currentWindow: true }, (tabs) => {
      if (tabs.length > 0) {
        openParabolicUrl(tabs[0].url);
      }
    });
  }
});