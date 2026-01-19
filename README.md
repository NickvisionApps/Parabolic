<div align="center">

![](resources/banner.png)

# 🎬 Parabolic

### *A powerful yt-dlp frontend*

[![Translation status](https://hosted.weblate.org/widgets/nickvision-tube-converter/-/app/svg-badge.svg)](https://hosted.weblate.org/engage/nickvision-tube-converter/)
[![Powered by Weblate](https://img.shields.io/badge/Powered%20by-Weblate-blue?style=flat-square)](https://weblate.org/en/)

[Features](#-features) •
[Installation](#-installation) •
[Screenshots](#-screenshots) •
[Building](#-building-manually) •
[Contributing](#-contributing)

</div>

---

## ✨ Features

<table>
<tr>
<td width="50%">

### 🌐 Versatile Downloads
Powerful frontend for **yt-dlp** with support for [hundreds of sites](https://github.com/yt-dlp/yt-dlp/blob/master/supportedsites.md)

### 🎵 Multiple Formats
Download in **mp4**, **webm**, **mp3**, **opus**, **flac**, and **wav**

</td>
<td width="50%">

### ⚡ Concurrent Downloads
Run **multiple downloads** at the same time

### 📝 Complete Metadata
Support for downloading **metadata** and **video subtitles**

</td>
</tr>
</table>

---

## ⚖️ Legal Copyright Disclaimer

> **Warning:** Videos on YouTube and other sites may be subject to DMCA protection. The authors of Parabolic do not endorse, and are not responsible for, the use of this application in means that will violate these laws.

---

## 📥 Installation

<div align="center">

### Choose your platform

<table>
<tr>
<td align="center" width="33%">

### Linux

<a href='https://flathub.org/apps/details/org.nickvision.tubeconverter'>
<img width='200' alt='Download on Flathub' src='https://flathub.org/api/badge?locale=en'/>
</a>

</td>
<td align="center" width="33%">

### Windows

<a href="https://github.com/NickvisionApps/Parabolic/releases">
<img width='180' alt="Download from Releases" src="https://upload.wikimedia.org/wikipedia/commons/e/e2/Windows_logo_and_wordmark_-_2021.svg"/>
</a>

Download the latest version from **Releases**

</td>
<td align="center" width="33%">

### Browser Extensions

[![get-the-addon](extension/resources/firefox.png)](https://addons.mozilla.org/en-US/firefox/addon/parabolic/)

**Chrome:** See [extension folder](https://github.com/NickvisionApps/Parabolic/tree/dotnet/extension) for manual installation instructions

</td>
</tr>
</table>

### 🌍 Translation Status

[![Translation Status](https://hosted.weblate.org/widget/nickvision-tube-converter/multi-auto.svg)](https://hosted.weblate.org/engage/nickvision-tube-converter/)

**Help us translate Parabolic!** [Click here to contribute](https://hosted.weblate.org/projects/nickvision-tube-converter/)

</div>

---

## 💬 Chat & Community

<div align="center">

Join our community on Matrix!

<a href='https://matrix.to/#/#nickvision:matrix.org'>
<img width='160' alt='Join our room' src='https://user-images.githubusercontent.com/17648453/196094077-c896527d-af6d-4b43-a5d8-e34a00ffd8f6.png'/>
</a>

</div>

---

## 🤝 Contributing

We'd love your contribution! See [**CONTRIBUTING.md**](CONTRIBUTING.md) for details on:

- 🐛 How to report bugs
- 💡 How to suggest new features
- 🔧 How to contribute code
- 📖 How to improve documentation

---

## 📸 Screenshots

<details>
<summary><b>🖥️ GNOME Interface</b></summary>

<br>

| Home Page | Active Downloads |
|:---:|:---:|
| ![Home Page](org.nickvision.tubeconverter.gnome/screenshots/home.png) | ![Downloading](org.nickvision.tubeconverter.gnome/screenshots/downloading.png) |

| Dark Mode | Add Download |
|:---:|:---:|
| ![Dark Mode](org.nickvision.tubeconverter.gnome/screenshots/dark.png) | ![Add Download Dialog](org.nickvision.tubeconverter.gnome/screenshots/add.png) |

</details>

<details>
<summary><b>🪟 Windows Interface (WinUI)</b></summary>

<br>

| Home Page | Active Downloads |
|:---:|:---:|
| ![Home Page](org.nickvision.tubeconverter.winui/screenshots/home.png) | ![Downloading](org.nickvision.tubeconverter.winui/screenshots/downloading.png) |

| Dark Mode | Add Download |
|:---:|:---:|
| ![Dark Mode](org.nickvision.tubeconverter.winui/screenshots/dark.png) | ![Add Download Dialog](org.nickvision.tubeconverter.winui/screenshots/add.png) |

</details>

<details>
<summary><b>🧩 Chrome Extension</b></summary>

<br>

<video width="800" height="600" controls>
  <source src="extension/resources/chrome.mp4" type="video/mp4">
</video>

</details>

<details>
<summary><b>🦊 Firefox Extension</b></summary>

<br>

<video width="800" height="600" controls>
  <source src="extension/resources/firefox.mp4" type="video/mp4">
</video>

</details>

---

## 🔨 Building

Parabolic is a .NET 10 project and can easily be built on any platform. Besides .NET 10, the following are required system dependencies for building each project:

### 📦 Dependencies

#### Shared
- [gettext](https://www.gnu.org/software/gettext/)
  - Can be installed on Windows using `msys2`
- [yelp-tools](https://wiki.gnome.org/Apps/Yelp/Tools)
  - Can be installed on Windows using `msys2`

#### WinUI
- [WindowsAppSDK](https://learn.microsoft.com/en-us/windows/apps/windows-app-sdk/)

#### GNOME
- [Gtk4](https://docs.gtk.org/gtk4/)
- [libadwaita](https://gitlab.gnome.org/GNOME/libadwaita)
- [blueprint-compiler](https://gitlab.gnome.org/GNOME/blueprint-compiler)

### 🏗️ Build Process

Once all dependencies are available on the system, simply run:

**WinUI:**
```bash
dotnet run --project Nickvision.Parabolic.WinUI
```

**GNOME:**
```bash
dotnet run --project Nickvision.Parabolic.GNOME
```

---

## 📜 Code of Conduct

This project follows the [**GNOME Code of Conduct**](https://conduct.gnome.org/).

We expect all participants to treat each other with respect and contribute to a welcoming and inclusive community.
