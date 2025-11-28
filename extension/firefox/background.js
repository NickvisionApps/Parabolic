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

// Creates the context menu item when the extension is installed
browser.runtime.onInstalled.addListener(() => {
  browser.contextMenus.create({
    id: "openParabolicLink",
    title: "Open link in Parabolic",
    contexts: ["page", "link"] // Shows the menu on the page and on links
  });
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