# Contributing to Nickvision Parabolic

First off, thanks for taking the time to contribute! ❤️

All types of contributions are encouraged and valued. See the [Table of Contents](#table-of-contents) for different ways to help and details about how this project handles them. Please make sure to read the relevant section before making your contribution. It will make it a lot easier for us maintainers and smooth out the experience for all involved. The community looks forward to your contributions. 🎉

> And if you like the project, but just don't have time to contribute, that's fine. There are other easy ways to support the project and show your appreciation, which we would also be very happy about:
> - Star the project
> - Tweet about it
> - Refer this project in your project's readme
> - [Sponsor](https://github.com/sponsors/nlogozzo) the lead developer

## Table of Contents

- [I Have a Question](#i-have-a-question)
- [I Want To Contribute](#i-want-to-contribute)
  - [Reporting Bugs](#reporting-bugs)
  - [Suggesting Enhancements/New Features](#suggesting-enhancements)
  - [Providing translations](#providing-translations)
    - [Via Weblate](#via-weblate)
    - [Manually](#manually)
  - [Your First Code Contribution](#your-first-code-contribution)
    - [Developing on Linux](#developing-on-linux)
    - [Developing on Windows](#developing-on-windows)
- [Styleguides](#styleguides)
- [Join The Project Team](#join-the-project-team)

## I Have a Question

Before you ask a question, it is best to search for existing [Discussions](https://github.com/NickvisionApps/Parabolic/discussions) and [Issues](https://github.com/NickvisionApps/Parabolic/issues) that might help you. In case you have found a suitable issue and still need clarification, you can write your question in this discussion post. It is also advisable to search the internet for answers first.

If you then still feel the need to ask a question and need clarification, we recommend the following:

- Open a [Discussion](https://github.com/NickvisionApps/Parabolic/discussions).
- Provide as much context as you can about what you're running into.
- Provide project and platform versions (windows, gnome, etc...), depending on what seems relevant.

We will then take care of the question as soon as possible.

## I Want To Contribute

> ### Legal Notice
> When contributing to this project, you must agree that you have authored 100% of the content, that you have the necessary rights to the content and that the content you contribute may be provided under the project [license](LICENSE).

### Reporting Bugs

#### Before Submitting a Bug Report

A good bug report shouldn't leave others needing to chase you up for more information. Therefore, we ask you to investigate carefully, collect information and describe the issue in detail in your report. Please complete the following steps in advance to help us fix any potential bug as fast as possible.

- Make sure that you are using the latest released version.
- Determine if your bug is really a bug and not an error on your side. If you are looking for support, you might want to check [this section](#i-have-a-question).
- To see if other users have experienced (and potentially already solved) the same issue you are having, check if there is not already a bug report existing for your bug or error in both the [Discussions](https://github.com/NickvisionApps/Parabolic/discussions) and [Issues](https://github.com/NickvisionApps/Parabolic/issues) sections.
- Collect information about the bug:
  - Debug information provided by the application
    - GNOME: from main menu open About Parabolic → Troubleshooting → Debugging Information, here you can copy information to clipboard or save to a file.
  - Stack trace (Traceback)
    - Including any error messages thrown by the application
    - You may need to start the application via the terminal/console to receive an error message for a crash.
  - OS, Platform and Version (Linux/Distro, Kernel Version, x64/ARM)
  - Possibly your input and the output
  - Can you reliably reproduce the issue? And can you also reproduce it with older versions?

#### How Do I Submit a Good Bug Report?

> You must never report security related issues, vulnerabilities or bugs including sensitive information to the issue tracker, or elsewhere in public. Instead sensitive bugs must be sent by email to <nlogozzo225@gmail.com>.

We use GitHub issues to track bugs and errors. If you run into an issue with the project:

- Open an [Issue](https://github.com/NickvisionApps/Parabolic/issues/new). (Since we can't be sure at this point whether it is a bug or not, we ask you not to talk about a bug yet and not to label the issue.)
- Explain the behavior you would expect and the actual behavior.
- Please provide as much context as possible and describe the *reproduction steps* that someone else can follow to recreate the issue on their own. This usually includes your code. For good bug reports you should isolate the problem and create a reduced test case.
- Provide the information you collected in the previous section.

Once it's filed:

- The project team will label the issue accordingly.
- A team member will try to reproduce the issue with your provided steps. If there are no reproduction steps or no obvious way to reproduce the issue, the team will ask you for those steps. Bugs that are not able to be reproduced will not be addressed until they are reproduced.
- If the team is able to reproduce the issue, it will be marked as a `bug` and the issue will be left to be [implemented by someone](#your-first-code-contribution).

### Suggesting Enhancements

This section guides you through submitting an enhancement suggestion for Nickvision Parabolic, **including completely new features and minor improvements to existing functionality**. Following these guidelines will help maintainers and the community to understand your suggestion and find related suggestions.

#### Before Submitting an Enhancement

- Make sure that you are using the latest released version.
- Perform a search through [Discussions](https://github.com/NickvisionApps/Parabolic/discussions) and [Issues](https://github.com/NickvisionApps/Parabolic/issues) to see if the enhancement has already been suggested. If it has, add a comment to the existing issue instead of opening a new one.
- Find out whether your idea fits with the scope and aims of the project. It's up to you to make a strong case to convince the project's developers of the merits of this feature. Keep in mind that we want features that will be useful to the majority of our users and not just a small subset.

#### How Do I Submit a Good Enhancement Suggestion?

Enhancement suggestions are tracked as [GitHub issues](https://github.com/NickvisionApps/Parabolic/issues).

- Use a **clear and descriptive title** for the issue to identify the suggestion.
- Provide a **step-by-step description of the suggested enhancement** in as many details as possible.
- **Describe the current behavior** and **explain which behavior you expected to see instead** and why. At this point you can also tell which alternatives do not work for you.
- You may want to **include screenshots and animated GIFs** which help you demonstrate the steps or point out the part which the suggestion is related to. You can use [this tool](https://www.cockos.com/licecap/) to record GIFs on macOS and Windows, and [this tool](https://github.com/colinkeenan/silentcast) or [this tool](https://flathub.org/apps/details/io.github.seadve.Kooha) on Linux.
- **Explain why this enhancement would be useful** to most Nickvision Parabolic users. You may also want to point out the other projects that solved it better and which could serve as inspiration.

### Providing Translations

Everyone is welcome to translate this app into their native or known languages, so that the application is accessible to everyone.

##### Via Weblate

Parabolic is available to translate on [Weblate](https://hosted.weblate.org/engage/nickvision-tube-converter/)!

##### Manually

To start translating the app, fork the repository and clone it locally.

Parabolic uses [gettext](https://www.gnu.org/software/gettext/manual/gettext.html#PO-Files) for translations. In the `NickvisionTubeConverter.Shared/Resources/po` you will find files that can be edited in your favourite `*.po` files editor or any text editor. If you want to create a new translation, copy `parabolic.pot` file, name the new file `<lang_code>.po`, where `<lang_code>` is the language code for your translation (usually it's 2 letters, but it also can be a locale code to differentiate between different version of the same language, for example `pt` and `pt_BR`) and edit this file. Also add the language code to `LINGUAS` file (please keep codes sorted alphabetically there).

To check your translation file, make sure your system is in the locale of the language you are translating and run the app. You should see your translated strings!

In case you run the app in GNOME Builder, it will force the app to run in en_US locale. To run the app in your locale without exporting and installing it, follow this steps:

1. Build the application
2. Press Ctrl+Alt+T to open a terminal inside the application environment
3. Run the application with the following command: `LC_ALL=<locale-code> /app/bin/org.nickvision.tubeconverter`, where `<locale-code>` is your system locale code (e.g. `it_IT.UTF8`).

Once all changes to your translated file are made, commit these changes and create a pull request to the project.

### Your First Code Contribution

#### Structure

Parabolic is built using .NET 7 and C#. With these technologies, Parabolic is built for GNOME (Linux) and Windows.
The solution is setup into 3 projects:
 - NickvisionTubeConverter.Shared
 - NickvisionTubeConverter.GNOME
 - NickvisionTubeConverter.WinUI

The whole solution utilizes the [MVC](https://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93controller) pattern for separating data and UI views.

##### NickvisionTubeConverter.Shared

This project contains all of the code used by all platforms of the app. 
- Models => The data driven objects of the application (i.e. Configuration, Database, etc...)
- Controllers => The objects used by UI views to receive and manipulate data from the Models
- Helpers => Useful objects and functions such as the Gettext functions for receiving translated strings throughout the app
- Resources => Strings, icons, and fonts used by the app

##### NickvisionTubeConverter.GNOME

This project contains all of the code used for the GNOME platform version of the app, including flathub manifest and desktop files.
Powered by the C# bindings for GTK4/Libadwaita: [gir.core](https://github.com/gircore/gir.core)
- Views => The views (pages, windows, dialogs) of the app that connect to the shared controllers
- Controls => Generic controls for the app
  - These controls should not be connected to a controller and should be able to be ported to any other application
- Helpers => Useful objects that are specific for GNOME platform version of the app
- Blueprints => UI files written in [Blueprint markup language](https://jwestman.pages.gitlab.gnome.org/blueprint-compiler/)

##### NickvisionTubeConverter.WinUI

This project contains all of the code used for the Windows platform version of the app, including installer scripts.
Powered by the [Windows App SDK](https://learn.microsoft.com/en-us/windows/apps/windows-app-sdk/)
- Views => The views (pages, windows, dialogs) of the app that connect to the shared controllers
- Controls => Generic controls for the app
  - These controls should not be connected to a controller and should be able to be ported to any other application
- Installer => Inno installer scripts

#### Developing on Linux

##### GNOME

Recommended IDEs:
- GNOME Builder 43 and up.
- VS Code with [flatpak extension](https://github.com/bilelmoussaoui/flatpak-vscode).

You may also make your changes via any code editor and use [`flatpak-builder`](https://docs.flatpak.org/en/latest/flatpak-builder.html) to run the application locally through flatpak.

You may also build the app manually without using flatpak. List of dependencies:
- dotnet >=7.0
- GTK >=4.12
- libadwaita >=1.4
- python3 >=3.7
- python3-psutil
- yt-dlp (as python module, standalone binary won't work)
- ffmpeg
- aria2 (optional)
- libunity (optional)
- [cake](https://cakebuild.net/) (build only)
    - `dotnet tool install --global Cake.Tool` or `dotnet tool restore` (in repository root folder)
- blueprint-compiler, GTK and libadwaita development files (build only)
- glib-compile-resources (build only)

Use one of the commands to build the app:

| Command | Result |
|---|---|
| `dotnet cake --target=Run --ui=gnome` (in repo root folder) or `dotnet run` (in project subfolder) | Builds the application and runs it. Application will not get installed, which might result in some missing icons and lack of desktop integration. |
| `dotnet cake --target=Publish --prefix=PREFIX --ui=gnome` | Builds the application in `_nickbuild` directory, preparing it to be installed in a provided prefix (examples of a valid prefix: `/usr`, `/app`). If `--self-contained` is added, the application will not need dotnet-runtime to run. 
| `dotnet cake --target=Install --destdir=DESTDIR` | Copies files to the `DESTDIR`. `--destdir` is optional, by default files are copied to root (`/`). This command should be used after `Publish`. |

##### WinUI

Can not be developed on non-Windows platforms.

#### Developing on Windows

##### WinUI
Recommended IDE:
- Visual Studio 2022 setup for use with Windows App SDK

You may also build the app manually without Visual Studio 2022 but the Windows App SDK runtime must be installed. It can be found here: https://learn.microsoft.com/en-us/windows/apps/windows-app-sdk/downloads.

You will also need gettext for Windows to be installed for the app to properly utilize transactions. Get it here: https://mlocati.github.io/articles/gettext-iconv-windows.html

With all dependencies installed a simple `dotnet run` will build and start the app.

##### GNOME

Can not be developed on non-Linux platforms.

## Styleguides

Parabolic follows [Microsoft's C# Coding Conventions](https://learn.microsoft.com/en-us/dotnet/csharp/fundamentals/coding-style/coding-conventions).

See [Microsoft's C# Identifier Names](https://learn.microsoft.com/en-us/dotnet/csharp/fundamentals/coding-style/identifier-names) as well.

## Join The Project Team

<a href='https://matrix.to/#/#nickvision:matrix.org'><img width='140' alt='Join our room' src='https://user-images.githubusercontent.com/17648453/196094077-c896527d-af6d-4b43-a5d8-e34a00ffd8f6.png'/></a>

## Attribution
This guide is based on the **contributing-gen**. [Make your own](https://github.com/bttger/contributing-gen)!
