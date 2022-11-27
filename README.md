# Tube Converter
<img src="src/resources/org.nickvision.tubeconverter.svg" width="100" height="100"/>

**An easy-to-use YouTube video downloader**

# Features
- A basic yt-dlp frontend
- Supports downloading videos in multiple formats (mp4, webm, mp3, opus, flac, and wav)
- Run multiple downloads at a time
- Supports downloading metadata and video subtitles

# Disclaimer
The authors of Nickvision Tube Converter are not responsible/liable for any misuse of this program that may violate local copyright/DMCA laws. Users use this application at their own risk.

# Installation
<a href='https://flathub.org/apps/details/org.nickvision.tubeconverter'><img width='140' alt='Download on Flathub' src='https://flathub.org/assets/badges/flathub-badge-en.png'/></a>

# Chat
<a href='https://matrix.to/#/#nickvision:matrix.org'><img width='140' alt='Join our room' src='https://user-images.githubusercontent.com/17648453/196094077-c896527d-af6d-4b43-a5d8-e34a00ffd8f6.png'/></a>

# Screenshots
![MainWindow](https://user-images.githubusercontent.com/17648453/194887430-b934194b-ad9f-4b42-a3e1-ef3b6a17aab4.png)
![AddDownloadDialog](https://user-images.githubusercontent.com/17648453/196213073-a321c459-96b3-4f11-a5e9-7352c4d7b6c4.png)
![Downloading](https://user-images.githubusercontent.com/17648453/196213082-e9b2bb79-a276-425f-9d3c-f93a8203e703.png)
![Done](https://user-images.githubusercontent.com/17648453/196213096-0522cf8e-41b4-4043-a1f7-8c0acd0c0e1d.png)
![DarkMode](https://user-images.githubusercontent.com/17648453/196213105-fe26ca19-cf68-40c3-87aa-e7d71a86c4ba.png)
![Logs](https://camo.githubusercontent.com/0d1b620a9cb25dc9e94c7c26a6d9d4647c9cc42647b6dafdf96c95f62f3ded9d/68747470733a2f2f692e696d6775722e636f6d2f304532753861622e706e67)

# Translating
Everyone is welcome to translate this app into their native or known languages, so that the application is accessible to everyone.

To translate the app, fork the repository and clone it locally. Make sure that `meson` is installed. Run the commands in your shell while in the directory of repository:
```bash
meson build
cd build
meson compile org.nickvision.tubeconverter-pot
```
Or, if you are using GNOME Builder, build the app and then run in the Builder's terminal:
```bash
flatpak run --command=sh org.gnome.Builder
cd _build
meson compile org.nickvision.tubeconverter-pot
```
This would generate a `NickvisionTubeConverter/po/org.nickvision.tubeconverter.pot` file, now you can use this file to translate the strings into your target language. You may use [Gtranslator](https://flathub.org/apps/details/org.gnome.Gtranslator) or [poedit](https://poedit.net) if you do not know how to translate manually in text itself. After translating (either through tools or directly in text editor), make sure to include the required metadata on the top of translation file (see existing files in `NickvisionTubeConverter/po/` directory.)

One particular thing you should keep in mind is that some strings in this project are bifurcated into multiple strings to cater to responsiveness of the application, like:
```
msgid ""
"If checked, the currency symbol will be displayed on the right of a monetary "
"value."
```
You should use the same format for translated strings as well. But, because all languages do not have the same sentence structure, you may not need to follow this word-by-word, rather you should bifurcate the string in about the same ratio. (For examples, look into translations of languages which do not have a English-like structure in `NickvisionTubeConverter/po/`)

Put your translated file in `NickvisionTubeConverter/po` directory in format `<LANG>.po` where `<LANG>` is the language code.

Put the language code of your language in `NickvisionTubeConverter/po/LINGUAS` (this file, as a convention, should remain in alphabetical order.)

Add information in `NickvisionTubeConverter/po/CREDITS.json` so your name will appear in the app's About dialog:
```
"Jango Fett": {
    "lang": "Mandalorian",
    "email": "jango@galaxyfarfar.away"
}
```
If you made multiple translations, use an array to list all languages:
```
"C-3PO": {
    "lang": ["Ewokese", "Wookieespeak", "Jawaese"],
    "url": "https://free.droids"
}
```

To test your translation in GNOME Builder, press Ctrl+Alt+T to open a terminal inside the app's environment and then run:
```
LC_ALL=<LOCALE> /app/bin/org.nickvision.tubeconverter
```
where `<LOCALE>` is your locale (e.g. `it_IT.UTF-8`.)

Commit these changes, and then create a pull request to the project.

As more strings may be added in the application in future, the following command needs to be ran to update all the `.po` files, which would add new strings to be translated without altering the already translated strings. But, because running this command would do this for all the languages, generally a maintainer would do that.

```bash
meson compile org.nickvision.tubeconverter-update-po
```

# Dependencies
- [C++20](https://en.cppreference.com/w/cpp/20)
- [GTK 4](https://www.gtk.org/)
- [libadwaita](https://gnome.pages.gitlab.gnome.org/libadwaita/)
- [jsoncpp](https://github.com/open-source-parsers/jsoncpp)

# Special Thanks
- [daudix-UFO](https://github.com/daudix-UFO) and [martin-desktops](https://github.com/martin-desktops) for our application icons
