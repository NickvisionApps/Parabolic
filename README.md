![](resources/banner.png)

[![Translation status](https://hosted.weblate.org/widgets/nickvision-tube-converter/-/app/svg-badge.svg)](https://hosted.weblate.org/engage/nickvision-tube-converter/) ✨Powered by [Weblate](https://weblate.org/en/)✨

# Features
- A powerful yt-dlp frontend ([supported sites](https://github.com/yt-dlp/yt-dlp/blob/master/supportedsites.md))
- Supports downloading videos in multiple formats (mp4, webm, mp3, opus, flac, and wav)
- Run multiple downloads at a time
- Supports downloading metadata and video subtitles

# Legal Copyright Disclaimer
Videos on YouTube and other sites may be subject to DMCA protection. The authors of Parabolic do not endorse, and are not responsible for, the use of this application in means that will violate these laws.

# Installation
<p><a href='https://flathub.org/apps/details/org.nickvision.tubeconverter'><img width='150' alt='Download on Flathub' src='https://flathub.org/api/badge?locale=en'/></a></p>
<p><a href="https://snapcraft.io/tube-converter"><img width='150' alt="Get it from the Snap Store" src="https://snapcraft.io/static/images/badges/en/snap-store-black.svg" /></a></p>
<p><a href="https://github.com/NickvisionApps/Parabolic/releases"><img width='140' alt="Download from Releases" src="https://upload.wikimedia.org/wikipedia/commons/e/e2/Windows_logo_and_wordmark_-_2021.svg"/></a></p>

[![get-the-addon](extension/resources/firefox.png)](https://addons.mozilla.org/en-US/firefox/addon/parabolic/)

# Chat
<a href='https://matrix.to/#/#nickvision:matrix.org'><img width='140' alt='Join our room' src='https://user-images.githubusercontent.com/17648453/196094077-c896527d-af6d-4b43-a5d8-e34a00ffd8f6.png'/></a>

# Contributing
See [CONTRIBUTING.md](CONTRIBUTING.md) for details on how can you help the project and how to provide information so we can help you in case of troubles with the app.

# Screenshots
<details>
 <summary>GNOME</summary>

 ![Home Page](org.nickvision.tubeconverter.gnome/screenshots/home.png)
 ![Downloading](org.nickvision.tubeconverter.gnome/screenshots/downloading.png)
 ![Dark Mode](org.nickvision.tubeconverter.gnome/screenshots/dark.png)
 ![Add Download Dialog](org.nickvision.tubeconverter.gnome/screenshots/add.png)
</details>

<details>
 <summary>WinUI</summary>

 ![Home Page](org.nickvision.tubeconverter.winui/screenshots/home.png)
 ![Downloading](org.nickvision.tubeconverter.winui/screenshots/downloading.png)
 ![Dark Mode](org.nickvision.tubeconverter.winui/screenshots/dark.png)
 ![Add Download Dialog](org.nickvision.tubeconverter.winui/screenshots/add.png)
</details>

<details>
  <summary>Chrome Extension</summary>

  <video width="800" height="600" controls>
    <source src="extension/resources/chrome.mp4" type="video/mp4">
  </video>
</details>

<details>
  <summary>Firefox Extension</summary>

  <video width="800" height="600" controls>
    <source src="extension/resources/firefox.mp4" type="video/mp4">
  </video>
</details>

# Building

Parabolic is a .NET 10 project and can easily be built on any platform. Besides, .NET 10 the following are required system dependencies for building each project:

- Shared
  - [gettext](https://www.gnu.org/software/gettext/)
    - Can be installed on Windows using `msys2`
  - [yelp-tools](https://wiki.gnome.org/Apps/Yelp/Tools)
    - Can be installed on Windows using `msys2`
- WinUI
  - [WindowsAppSDK](https://learn.microsoft.com/en-us/windows/apps/windows-app-sdk/)
- GNOME
  - [Gtk4](https://docs.gtk.org/gtk4/)
  - [libadwaita](https://gitlab.gnome.org/GNOME/libadwaita)
  - [blueprint-compiler](https://gitlab.gnome.org/GNOME/blueprint-compiler)

Once all dependencies are available on the system, simply run `dotnet run --project Nickvision.Parabolic.WinUI` or `dotnet run --project Nickvision.Parabolic.GNOME` to run the version of the app for your system.

# Code of Conduct

This project follows the [GNOME Code of Conduct](https://conduct.gnome.org/).