<p align="center">
    <img src=".github/img/logo.svg"/>
</p>

![spotify-qt with dark theme](.github/img/spotify-qt.png)

[![Linux Build](https://github.com/kraxarn/spotify-qt/actions/workflows/linux.yml/badge.svg)](https://github.com/kraxarn/spotify-qt/actions/workflows/linux.yml)
[![Windows Build](https://github.com/kraxarn/spotify-qt/actions/workflows/windows-qt6-x64.yml/badge.svg)](https://github.com/kraxarn/spotify-qt/actions/workflows/windows-qt6-x64.yml)
[![macOS Build](https://github.com/kraxarn/spotify-qt/actions/workflows/macos.yml/badge.svg)](https://github.com/kraxarn/spotify-qt/actions/workflows/macos.yml)

An unofficial Spotify client using Qt as a simpler, lighter alternative to the official client. Please note that you
need an actual Spotify client running, for example [librespot](https://github.com/librespot-org/librespot), which can be
configured from within the app.

> [!NOTE]
> Controlling playback requires Spotify Premium.

#### Contributions are very welcome!

## Goals

* Fast, light on resources, and small file size.
* Portable, and supporting as many platforms and architectures as possible.
* Customizable.

## Non-goals

* Having the same features as the official client.

## Supported platforms

Any system with a C++17 compiler and Qt 6 is supported, but only Linux (x64) and macOS (arm64)
are regularly being tested, so if an issue occurs on your favorite platform, please report it.

## Installing

spotify-qt is available from some package managers. If there are any package issues, or the version
is out of date, contact the respective package maintainers, not me, unless I'm the package
maintainer. If you want to maintain the package on a system not in the list below, you are very much
free to, just please open an issue, or get in contact with me, so I can add it to the list.

| Platform             | Package                                                                                                                                                                                                                                                     | Maintainer                                          |
|----------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------|
| Arch (or Arch-based) | [![AUR](https://repology.org/badge/version-for-repo/aur/spotify-qt.svg?header=AUR)](https://aur.archlinux.org/packages/spotify-qt)                                                                                                                          | [kraxarn](https://github.com/kraxarn)               |
| NetBSD               | [![pkgsrc](https://repology.org/badge/version-for-repo/pkgsrc_current/spotify-qt.svg?header=pkgsrc)](https://pkgsrc.se/audio/spotify-qt)                                                                                                                    | [pin](https://codeberg.org/pin)                     |
| FreeBSD              | [![FreeBSD Ports](https://repology.org/badge/version-for-repo/freebsd/spotify-qt.svg?header=FreeBSD%20Ports)](https://www.freshports.org/audio/spotify-qt)                                                                                                  | [ehaupt](https://github.com/ehaupt)                 |
| Void Linux           | [![Void Linux](https://repology.org/badge/version-for-repo/void_x86_64/spotify-qt.svg?header=Void%20Linux)](https://github.com/void-linux/void-packages/blob/master/srcpkgs/spotify-qt/template)                                                            | [abenson](https://github.com/abenson)               |
| NixOS                | [![nixpkgs unstable](https://repology.org/badge/version-for-repo/nix_unstable/spotify-qt.svg?header=nixpkgs%20unstable)](https://github.com/NixOS/nixpkgs/blob/master/pkgs/applications/audio/spotify-qt/default.nix)                                       | [karthikiyengar](https://github.com/karthikiyengar) |
| openSUSE             | [![openSUSE Multimedia:Apps Tumbleweed package](https://repology.org/badge/version-for-repo/opensuse_multimedia_apps_tumbleweed/spotify-qt.svg?header=openSUSE)](https://software.opensuse.org//download.html?project=multimedia%3Aapps&package=spotify-qt) | [KaratekHD](https://github.com/KaratekHD)           |
| Alpine Linux         | [![Alpine Linux Edge](https://repology.org/badge/version-for-repo/alpine_edge/spotify-qt.svg?header=Alpine%20Linux%20Edge)](https://pkgs.alpinelinux.org/packages?name=spotify-qt)                                                                          | [ptrcnull](https://github.com/ptrcnull)             |
| Fedora               | [![Copr](https://repology.org/badge/version-for-repo/aur/spotify-qt.svg?header=Copr)](https://copr.fedorainfracloud.org/coprs/kraxarn/spotify-qt)                                                                                                           | [kraxarn](https://github.com/kraxarn)               |
| Windows              | [![Scoop extras](https://repology.org/badge/version-for-repo/scoop/spotify-qt.svg?header=Scoop%20extras)](https://github.com/ScoopInstaller/Extras/blob/master/bucket/spotify-qt.json)                                                                      | [brian6932](https://github.com/brian6932)           |

If you're running some other Linux distribution, macOS, or Windows, you can instead download portable binaries from the
[release page](https://github.com/kraxarn/spotify-qt/releases/latest). If you want to try out new features, you can
download unstable builds automatically built every day
from [spotify-qt-nightly](https://kraxarn.github.io/spotify-qt-nightly).

## Questions?

Check the [FAQ on the wiki](https://github.com/kraxarn/spotify-qt/wiki/Frequently-Asked-Questions),
or the [discussions](https://github.com/kraxarn/spotify-qt/discussions)!

You can also check out the [roadmap](https://github.com/users/kraxarn/projects/2),
or [issues](https://github.com/kraxarn/spotify-qt/issues), for planned features and known bugs.

## Building yourself

Make sure you have Qt (6.0+) with SVG support, CMake (3.16+) and various build tools like
`make`, `g++` and `git`. Keep in mind that the master branch is unstable at all times. Therefore,
it's recommended to use the stable branch, which is updated after each new stable release, only
including important bug fixes, unless you want to try out new features.

```
git clone --branch stable https://github.com/kraxarn/spotify-qt.git
cd spotify-qt
mkdir build
cd build
cmake ..
make
./spotify-qt
```
